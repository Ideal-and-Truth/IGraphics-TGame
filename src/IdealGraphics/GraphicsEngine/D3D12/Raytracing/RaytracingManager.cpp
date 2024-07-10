#include "RaytracingManager.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructureManager.h"
#include "GraphicsEngine/D3D12/D3D12UploadBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12SRV.h"
#include "GraphicsEngine/D3D12/D3D12UAV.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorManager.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/VertexInfo.h"

#include "GraphicsEngine/D3D12/D3D12Texture.h"

#include "GraphicsEngine/Resource/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/IdealMaterial.h"

#include "GraphicsEngine/Resource/IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/IdealSkinnedMesh.h"
#include "GraphicsEngine/Resource/IdealMesh.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"

#include "GraphicsEngine/D3D12/D3D12RayTracingRenderer.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/DeferredDeleteManager.h"

#include <d3d12.h>
#include <d3dx12.h>

inline void CreateSRV(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12Resource> Resource, D3D12_CPU_DESCRIPTOR_HANDLE Handle, uint32 NumElements, uint32 ElementSize)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//srvDesc.Format = Resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = NumElements;
	srvDesc.Buffer.StructureByteStride = ElementSize;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	Device->CreateShaderResourceView(Resource.Get(), &srvDesc, Handle);
}

inline void SerializeAndCreateRootSignature(ComPtr<ID3D12Device5> Device, D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSignature, LPCWSTR ResourceName = nullptr)
{
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;

	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
	if (error)
	{
		const wchar_t* msg = static_cast<wchar_t*>(error->GetBufferPointer());
		Check(hr, msg);
	}

	Check(
		Device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootSignature))),
		L"Failed to create Rootsignature"
		);


	if (ResourceName)
	{
		(*rootSignature)->SetName(ResourceName);
	}
}

Ideal::RaytracingManager::RaytracingManager()
{

}

Ideal::RaytracingManager::~RaytracingManager()
{

}

void Ideal::RaytracingManager::Init(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12Shader> RaytracingShader, std::shared_ptr<Ideal::D3D12Shader> AnimationShader, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, uint32 Width, uint32 Height)
{
	m_width = Width;
	m_height = Height;
	m_ASManager = std::make_unique<DXRAccelerationStructureManager>();
	m_uavSingleDescriptorHeap = std::make_shared<Ideal::D3D12DescriptorHeap>();
	m_uavSingleDescriptorHeap->Create(Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
	
	CreateUAVRenderTarget(Device, Width, Height);	//device, width, height
	CreateRootSignature(Device);	//device
	CreateRaytracingPipelineStateObject(Device, RaytracingShader); // device, shader
	BuildShaderTables(Device, ResourceManager, DescriptorManager, nullptr); // Device, ResourceManager

	//return;
	//-----Animation Pipeline-----//
	CreateAnimationRootSignature(Device);
	CreateAnimationCSPipelineState(Device, AnimationShader);
}

void Ideal::RaytracingManager::DispatchRays(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, uint32 CurrentFrameIndex, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, SceneConstantBuffer SceneCB, std::shared_ptr<Ideal::D3D12Texture> SkyBoxTexture)
{
	CommandList->SetPipelineState1(m_dxrStateObject.Get());
	CommandList->SetComputeRootSignature(m_raytracingGlobalRootSignature.Get());
	CommandList->SetDescriptorHeaps(1, DescriptorManager->GetDescriptorHeap().GetAddressOf());

	// TODO : Global Root 연결해주는 부분 나중에 뺄 것
	
	// Parameter 0
	auto handle0 = DescriptorManager->Allocate(CurrentFrameIndex);
	//Device->CopyDescriptorsSimple(1, handle0.GetCpuHandle(), OutputUAVHandle.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Device->CopyDescriptorsSimple(1, handle0.GetCpuHandle(), m_raytacingOutputResourceUAVCpuDescriptorHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(Ideal::GlobalRootSignature::Slot::UAV_Output, handle0.GetGpuHandle());

	// Parameter 1
	CommandList->SetComputeRootShaderResourceView(Ideal::GlobalRootSignature::Slot::SRV_AccelerationStructure, m_ASManager->GetTLASResource()->GetGPUVirtualAddress());

	// Parameter 2
	auto cb = CBPool->Allocate(Device.Get(), sizeof(SceneConstantBuffer));
	memcpy(cb->SystemMemAddr, &SceneCB, sizeof(SceneCB));
	auto handle2 = DescriptorManager->Allocate(CurrentFrameIndex);
	Device->CopyDescriptorsSimple(1, handle2.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(Ideal::GlobalRootSignature::Slot::CBV_Global, handle2.GetGpuHandle());

	// Parameter 3
	if (SkyBoxTexture)
	{
		auto handle3 = DescriptorManager->Allocate(CurrentFrameIndex);
		Device->CopyDescriptorsSimple(1, handle3.GetCpuHandle(), SkyBoxTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CommandList->SetComputeRootDescriptorTable(Ideal::GlobalRootSignature::Slot::SRV_SkyBox, handle3.GetGpuHandle());
	}

	//-----------------Dispatch Rays----------------//

	D3D12_DISPATCH_RAYS_DESC dispatchRayDesc = {};
	dispatchRayDesc.HitGroupTable.StartAddress = m_hitGroupShaderTable->GetGPUVirtualAddress();
	dispatchRayDesc.HitGroupTable.SizeInBytes = m_hitGroupShaderTable->GetDesc().Width;
	dispatchRayDesc.HitGroupTable.StrideInBytes = m_hitGroupShaderTableStrideInBytes;

	dispatchRayDesc.MissShaderTable.StartAddress = m_missShaderTable->GetGPUVirtualAddress();
	dispatchRayDesc.MissShaderTable.SizeInBytes = m_missShaderTable->GetDesc().Width;
	dispatchRayDesc.MissShaderTable.StrideInBytes = m_missShaderTableStrideInBytes;

	dispatchRayDesc.RayGenerationShaderRecord.StartAddress = m_rayGenShaderTable->GetGPUVirtualAddress();
	dispatchRayDesc.RayGenerationShaderRecord.SizeInBytes = m_rayGenShaderTable->GetDesc().Width;
	
	dispatchRayDesc.Width = m_width;
	dispatchRayDesc.Height = m_height;
	dispatchRayDesc.Depth = 1;

	//CommandList->SetPipelineState1(m_dxrStateObject.Get());
	CommandList->DispatchRays(&dispatchRayDesc);
}

void Ideal::RaytracingManager::Resize(ComPtr<ID3D12Device5> Device, uint32 Width, uint32 Height)
{
	m_width = Width;
	m_height = Height;
	m_uavSingleDescriptorHeap->Reset();
	CreateUAVRenderTarget(Device, Width, Height);
}

void Ideal::RaytracingManager::CreateUAVRenderTarget(ComPtr<ID3D12Device5> Device, const uint32& Width, const uint32& Height)
{
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, Width, Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	Check(
		Device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			IID_PPV_ARGS(&m_raytracingOutput)
		),
		L"Failed to create Raytracing Output Resource"
	);
	m_raytracingOutput->SetName(L"RaytracingOutput");

	auto handle = m_uavSingleDescriptorHeap->Allocate();
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	Device->CreateUnorderedAccessView(m_raytracingOutput.Get(), nullptr, &uavDesc, handle.GetCpuHandle());
	m_raytacingOutputResourceUAVCpuDescriptorHandle = handle.GetCpuHandle();
}

Microsoft::WRL::ComPtr<ID3D12Resource> Ideal::RaytracingManager::GetRaytracingOutputResource()
{
	return m_raytracingOutput;
}

std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> Ideal::RaytracingManager::GetBLASByName(const std::wstring& Name)
{
	auto ret = m_ASManager->GetBLAS(Name);

	return ret;
}

std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> Ideal::RaytracingManager::AddBLAS(std::shared_ptr<Ideal::D3D12RayTracingRenderer> Renderer, ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, std::shared_ptr<Ideal::IMeshObject> MeshObject, const wchar_t* Name, bool IsSkinnedData /*= false*/)
{
	std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> blas = nullptr;
	//if (!IsSkinnedData)
	//{
	//	blas = m_ASManager->GetBLAS(Name);
	//	// 이미 있다!
	//	if (blas != nullptr)
	//	{
	//		std::shared_ptr<Ideal::IdealStaticMeshObject> staticMeshObject = std::static_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject);
	//		staticMeshObject->SetBLAS(blas);
	//		return blas;
	//	}
	//}
	// 스키닝 데이터일 경우에는 그냥 BLAS를 새로 만든다.
	std::vector<Ideal::BLASGeometry> Geometries;
	if (IsSkinnedData)
	{
		std::shared_ptr<Ideal::IdealSkinnedMeshObject> skinnedMeshObject = std::static_pointer_cast<Ideal::IdealSkinnedMeshObject>(MeshObject);
		std::shared_ptr<Ideal::IdealSkinnedMesh> skinnedMesh = skinnedMeshObject->GetSkinnedMesh();

		skinnedMeshObject->CreateUAVVertexBuffer(Device, ResourceManager);
		uint32 numMesh = skinnedMesh->GetMeshes().size();
		Geometries.resize(numMesh);
		for (uint32 i = 0; i < numMesh; ++i)
		{
			Ideal::BLASGeometry blasGeometry;
			blasGeometry.Name = L"TEMP_SKINNED_BLAS_GEOMETRY";
			blasGeometry.VertexBufferResource = skinnedMeshObject->GetUAV_VertexBuffer()->GetResource();
			blasGeometry.VertexBufferGPUAddress = skinnedMeshObject->GetUAV_VertexBuffer()->GetResource()->GetGPUVirtualAddress();
			blasGeometry.VertexStrideInBytes = sizeof(BasicVertex);
			blasGeometry.VertexCount = skinnedMesh->GetMeshes()[i]->GetVertexBuffer()->GetElementCount();

			blasGeometry.IndexBufferResource = skinnedMesh->GetMeshes()[i]->GetIndexBuffer()->GetResource();
			blasGeometry.IndexBufferGPUAddress = skinnedMesh->GetMeshes()[i]->GetIndexBuffer()->GetResource()->GetGPUVirtualAddress();
			blasGeometry.IndexCount = skinnedMesh->GetMeshes()[i]->GetIndexBuffer()->GetElementCount();

			std::shared_ptr<Ideal::IdealMaterial> material = skinnedMesh->GetMeshes()[i]->GetMaterial();
			if (material)
			{
				std::shared_ptr<Ideal::D3D12Texture> albedoTexture = material->GetDiffuseTexture();
				if (albedoTexture)
				{
					blasGeometry.SRV_Diffuse = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
					Device->CopyDescriptorsSimple(1, blasGeometry.SRV_Diffuse.GetCpuHandle(), albedoTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}

				std::shared_ptr<Ideal::D3D12Texture> normalTexture = material->GetNormalTexture();
				if (normalTexture)
				{
					blasGeometry.SRV_Normal = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
					Device->CopyDescriptorsSimple(1, blasGeometry.SRV_Normal.GetCpuHandle(), normalTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}

				std::shared_ptr<Ideal::D3D12Texture> metalicTexture = material->GetMetallicTexture();
				if (metalicTexture)
				{
					blasGeometry.SRV_Metallic = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
					Device->CopyDescriptorsSimple(1, blasGeometry.SRV_Metallic.GetCpuHandle(), metalicTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}

				std::shared_ptr<Ideal::D3D12Texture> roughnessTexture = material->GetRoughnessTexture();
				if (roughnessTexture)
				{
					blasGeometry.SRV_Roughness= DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
					Device->CopyDescriptorsSimple(1, blasGeometry.SRV_Roughness.GetCpuHandle(), roughnessTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}
			blasGeometry.SRV_VertexBuffer = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
			CreateSRV(Device, skinnedMeshObject->GetUAV_VertexBuffer()->GetResource(), blasGeometry.SRV_VertexBuffer.GetCpuHandle(), skinnedMesh->GetMeshes()[i]->GetVertexBuffer()->GetElementCount(), sizeof(BasicVertex));
			blasGeometry.SRV_IndexBuffer = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
			CreateSRV(Device, skinnedMesh->GetMeshes()[i]->GetIndexBuffer()->GetResource(), blasGeometry.SRV_IndexBuffer.GetCpuHandle(), skinnedMesh->GetMeshes()[i]->GetIndexBuffer()->GetElementCount(), sizeof(uint32));

			Geometries[i] = blasGeometry;

			int a = 3;
		}
		blas = m_ASManager->AddBLAS(Renderer, Device.Get(), Geometries, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE, true, Name, IsSkinnedData);
		skinnedMeshObject->SetBLAS(blas);
		// 현재 BLAS안에 들어있는 Geometry의 개수만큼 contributionToHitGroupIndex를 늘려준다.
		//skinnedMeshObject->SetBLASInstanceIndex(instanceIndex);
	}
	else
	{
		std::shared_ptr<Ideal::IdealStaticMeshObject> meshObject = std::static_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject);
		std::shared_ptr<Ideal::IdealStaticMesh> mesh = meshObject->GetStaticMesh();
		uint32 numMesh = mesh->GetMeshes().size();
		Geometries.resize(numMesh);
		for (uint32 i = 0; i < numMesh; ++i)
		{
			Ideal::BLASGeometry blasGeometry;
			blasGeometry.Name = L"TEMP_BLAS_GEOMETRY";
			blasGeometry.VertexBufferResource = mesh->GetMeshes()[i]->GetVertexBuffer()->GetResource();
			blasGeometry.VertexBufferGPUAddress = mesh->GetMeshes()[i]->GetVertexBuffer()->GetResource()->GetGPUVirtualAddress();
			blasGeometry.VertexStrideInBytes = sizeof(BasicVertex);
			blasGeometry.VertexCount = mesh->GetMeshes()[i]->GetVertexBuffer()->GetElementCount();

			blasGeometry.IndexBufferResource = mesh->GetMeshes()[i]->GetIndexBuffer()->GetResource();
			blasGeometry.IndexBufferGPUAddress = mesh->GetMeshes()[i]->GetIndexBuffer()->GetResource()->GetGPUVirtualAddress();
			blasGeometry.IndexCount = mesh->GetMeshes()[i]->GetIndexBuffer()->GetElementCount();

			std::shared_ptr<Ideal::IdealMaterial> material = mesh->GetMeshes()[i]->GetMaterial();
			if (material)
			{
				std::shared_ptr<Ideal::D3D12Texture> diffuseTexture = material->GetDiffuseTexture();
				if (diffuseTexture)
				{
					//blasGeometry.DiffuseTexture = diffuseTexture->GetSRV();
					blasGeometry.SRV_Diffuse = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
					Device->CopyDescriptorsSimple(1, blasGeometry.SRV_Diffuse.GetCpuHandle(), diffuseTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				std::shared_ptr<Ideal::D3D12Texture> normalTexture = material->GetNormalTexture();
				if (normalTexture)
				{
					blasGeometry.SRV_Normal = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
					Device->CopyDescriptorsSimple(1, blasGeometry.SRV_Normal.GetCpuHandle(), normalTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}

				std::shared_ptr<Ideal::D3D12Texture> metalicTexture = material->GetMetallicTexture();
				if (metalicTexture)
				{
					blasGeometry.SRV_Metallic = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
					Device->CopyDescriptorsSimple(1, blasGeometry.SRV_Metallic.GetCpuHandle(), metalicTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}

				std::shared_ptr<Ideal::D3D12Texture> roughnessTexture = material->GetRoughnessTexture();
				if (roughnessTexture)
				{
					blasGeometry.SRV_Roughness = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
					Device->CopyDescriptorsSimple(1, blasGeometry.SRV_Roughness.GetCpuHandle(), roughnessTexture->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}
			blasGeometry.SRV_VertexBuffer = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
			CreateSRV(Device, mesh->GetMeshes()[i]->GetVertexBuffer()->GetResource(), blasGeometry.SRV_VertexBuffer.GetCpuHandle(), mesh->GetMeshes()[i]->GetVertexBuffer()->GetElementCount(), sizeof(BasicVertex));
			blasGeometry.SRV_IndexBuffer = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
			CreateSRV(Device, mesh->GetMeshes()[i]->GetIndexBuffer()->GetResource(), blasGeometry.SRV_IndexBuffer.GetCpuHandle(), mesh->GetMeshes()[i]->GetIndexBuffer()->GetElementCount(), sizeof(uint32));

			Geometries[i] = blasGeometry;
		}
		blas = m_ASManager->AddBLAS(Renderer, Device.Get(), Geometries, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE, false, Name, IsSkinnedData);
		meshObject->SetBLAS(blas);
	}

	uint64 geometrySizeInBLAS = Geometries.size();
	blas->SetInstanceContributionToHitGroupIndex(m_contributionToHitGroupIndexCount);
	m_contributionToHitGroupIndexCount += geometrySizeInBLAS;
	
	// blas ref count 를 증가시킨다.
	blas->AddRefCount();

	return blas;
}

std::shared_ptr<Ideal::BLASInstanceDesc> Ideal::RaytracingManager::AllocateInstanceByBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, uint32 InstanceContributionToHitGroupIndex /*= UINT_MAX*/, Matrix transform /*= Matrix::Identity*/, BYTE InstanceMask /*= 1*/)
{
	auto blasInstanceDesc = m_ASManager->AddInstanceByBLAS(BLAS, InstanceContributionToHitGroupIndex, transform, InstanceMask);
	return blasInstanceDesc;
}


void Ideal::RaytracingManager::DeleteBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, const std::wstring& Name, bool IsSkinnedData)
{
	uint64 geometrySizeInBLAS = BLAS->GetGeometries().size();
	m_contributionToHitGroupIndexCount -= geometrySizeInBLAS;
	m_ASManager->DeleteBLAS(BLAS, Name, IsSkinnedData);
}

void Ideal::RaytracingManager::DeleteBLASInstance(std::shared_ptr<Ideal::BLASInstanceDesc> Instance)
{
	m_ASManager->DeleteBLASInstance(Instance);
}

void Ideal::RaytracingManager::SetGeometryTransformByIndex(uint32 InstanceIndex, const Matrix& Transform)
{
	Ideal::DXRInstanceDesc* instance = m_ASManager->GetInstanceByIndex(InstanceIndex);
	instance->SetTransform(Transform);
}

void Ideal::RaytracingManager::FinalCreate2(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, bool ForceBuild /*= false*/)
{
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	//buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	m_ASManager->InitTLAS(Device.Get(), buildFlags, false, L"RaytracingManager TLAS");	// AllowUpdate false
	//m_ASManager->Build2(Device, CommandList, UploadBufferPool, nullptr, ForceBuild);
}

void Ideal::RaytracingManager::UpdateAccelerationStructures(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager)
{
	m_ASManager->Build(Device, CommandList, UploadBufferPool, DeferredDeleteManager);
}

void Ideal::RaytracingManager::CreateRootSignature(ComPtr<ID3D12Device5> Device)
{
	using namespace Ideal;
	//-------------Global Root Signature--------------//
	{
		CD3DX12_DESCRIPTOR_RANGE ranges[GlobalRootSignature::Slot::Count];
		ranges[GlobalRootSignature::Slot::UAV_Output].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);	// u0 : Output
		ranges[GlobalRootSignature::Slot::CBV_Global].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : Global
		ranges[GlobalRootSignature::Slot::SRV_SkyBox].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);	// t1 : SkyBox

		// binding
		CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignature::Slot::Count];
		rootParameters[GlobalRootSignature::Slot::UAV_Output].InitAsDescriptorTable(1, &ranges[GlobalRootSignature::Slot::UAV_Output]);
		rootParameters[GlobalRootSignature::Slot::CBV_Global].InitAsDescriptorTable(1, &ranges[GlobalRootSignature::Slot::CBV_Global]);
		rootParameters[GlobalRootSignature::Slot::SRV_SkyBox].InitAsDescriptorTable(1, &ranges[GlobalRootSignature::Slot::SRV_SkyBox]);

		// init as
		rootParameters[GlobalRootSignature::Slot::SRV_AccelerationStructure].InitAsShaderResourceView(0);	// t0

		CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] =
		{
			// LinearWrapSampler
			CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_ANISOTROPIC),	// s0
		};

		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(
			ARRAYSIZE(rootParameters), rootParameters, ARRAYSIZE(staticSamplers), staticSamplers
		);
		SerializeAndCreateRootSignature(Device, globalRootSignatureDesc, &m_raytracingGlobalRootSignature, L"Global Root Signature");
	}

	//-------------Local Root Signature--------------//
	{
		const uint32 space = 1;
		CD3DX12_DESCRIPTOR_RANGE ranges[LocalRootSignature::Slot::Count];
		ranges[LocalRootSignature::Slot::SRV_IndexBuffer].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, space);	// t0 : Indices
		ranges[LocalRootSignature::Slot::SRV_VertexBuffer].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, space);	// t1 : Vertices
		ranges[LocalRootSignature::Slot::SRV_Diffuse].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, space);	// t2 : Diffuse
		ranges[LocalRootSignature::Slot::SRV_Normal].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, space);	// t3 : Normal
		ranges[LocalRootSignature::Slot::SRV_Metalic].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, space);	// t4 : Metalic
		ranges[LocalRootSignature::Slot::SRV_Roughness].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5, space);	// t5 : Roughness


		CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignature::Slot::Count];
		rootParameters[LocalRootSignature::Slot::SRV_IndexBuffer].InitAsDescriptorTable(1, &ranges[LocalRootSignature::Slot::SRV_IndexBuffer]);
		rootParameters[LocalRootSignature::Slot::SRV_VertexBuffer].InitAsDescriptorTable(1, &ranges[LocalRootSignature::Slot::SRV_VertexBuffer]);
		rootParameters[LocalRootSignature::Slot::SRV_Diffuse].InitAsDescriptorTable(1, &ranges[LocalRootSignature::Slot::SRV_Diffuse]);
		rootParameters[LocalRootSignature::Slot::SRV_Normal].InitAsDescriptorTable(1, &ranges[LocalRootSignature::Slot::SRV_Normal]);
		rootParameters[LocalRootSignature::Slot::SRV_Metalic].InitAsDescriptorTable(1, &ranges[LocalRootSignature::Slot::SRV_Metalic]);
		rootParameters[LocalRootSignature::Slot::SRV_Roughness].InitAsDescriptorTable(1, &ranges[LocalRootSignature::Slot::SRV_Roughness]);

		CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		SerializeAndCreateRootSignature(Device, localRootSignatureDesc, &m_raytracingLocalRootSignature, L"Local Root Signature");
	}
}

void Ideal::RaytracingManager::CreateRaytracingPipelineStateObject(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::D3D12Shader> Shader)
{
	CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

	//----------------DXIL library----------------//
	CD3DX12_DXIL_LIBRARY_SUBOBJECT* lib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	D3D12_SHADER_BYTECODE libxil = CD3DX12_SHADER_BYTECODE(Shader->GetBufferPointer(), Shader->GetSize());
	lib->SetDXILLibrary(&libxil);
	lib->DefineExport(c_raygenShaderName);
	lib->DefineExport(c_closestHitShaderName);
	lib->DefineExport(c_missShaderName);

	CD3DX12_HIT_GROUP_SUBOBJECT* hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
	hitGroup->SetHitGroupExport(c_hitGroupName);
	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	// ray payload와 attribute structure의 최대 크기를 바이트 단위로 정의
	CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT* shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	uint32 payloadSize = sizeof(Color);
	uint32 attributesSize = sizeof(Vector2);
	shaderConfig->Config(payloadSize, attributesSize);

	//------------Local Root Signature------------//
	CreateLocalRootSignatureSubobjects(&raytracingPipeline);

	//------------Global Root Signature------------//
	CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT* globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature.Get());

	//------------Pipeline Configuration------------//
	CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT* pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	uint32 maxRecursionDepth = MAX_RAY_RECURSION_DEPTH;
	pipelineConfig->Config(maxRecursionDepth);

	Check(
		Device->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(m_dxrStateObject.GetAddressOf())),
		L"Failed to create DirectX Raytracing State Object!"
	);
}

void Ideal::RaytracingManager::CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
{
	CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT* localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
	localRootSignature->SetRootSignature(m_raytracingLocalRootSignature.Get());

	// local root signature로 subobject 설정
	CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT* rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
	rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
	rootSignatureAssociation->AddExport(c_hitGroupName);
}

void Ideal::RaytracingManager::BuildShaderTables(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager)
{
	// Reset
	//DescriptorManager->ResetFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);

	void* rayGenShaderIdentifier;
	void* missShaderIdentifier;
	void* hitGroupShaderIdentifier;

	uint32 shaderIdentifierSize = 0;
	ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
	Check(m_dxrStateObject.As(&stateObjectProperties), L"Failed to Get Interface State Object Properties");
	shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	// Get Shader Identifier
	rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_raygenShaderName);
	missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_missShaderName);
	hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_hitGroupName);

	// RayGen Shader Table
	{
		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize;
		Ideal::DXRShaderTable rayGenShaderTable(Device.Get(), numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
		rayGenShaderTable.push_back(Ideal::DXRShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize));

		if (DeferredDeleteManager)
		{
			DeferredDeleteManager->AddD3D12ResourceToDelete(m_rayGenShaderTable);
		}
		m_rayGenShaderTable = rayGenShaderTable.GetResource();
	}

	// Miss Shader Table
	{
		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize;
		Ideal::DXRShaderTable missShaderTable(Device.Get(), numShaderRecords, shaderRecordSize, L"MissShaderTable");
		missShaderTable.push_back(Ideal::DXRShaderRecord(missShaderIdentifier, shaderIdentifierSize));
		if (DeferredDeleteManager)
		{
			DeferredDeleteManager->AddD3D12ResourceToDelete(m_missShaderTable);
		}
		m_missShaderTable = missShaderTable.GetResource();
		m_missShaderTableStrideInBytes = missShaderTable.GetShaderRecordSize();
	}

	// Hit Shader Table
	{
		uint32 numShaderRecords = (uint32)m_contributionToHitGroupIndexCount;
		if (numShaderRecords == 0) numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize + sizeof(Ideal::LocalRootSignature::RootArgument);
		Ideal::DXRShaderTable hitGroupShaderTable(Device.Get(), numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");

		const std::vector<std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure>> BLASs = m_ASManager->GetBLASs();
		for (auto& blas : BLASs)
		{
			// blas가 가지고 있는 geometry들을 가져온다.
			const std::vector<BLASGeometry> blasGeometries = blas->GetGeometries();
			for (const BLASGeometry& blasGeometry : blasGeometries)
			{
				Ideal::LocalRootSignature::RootArgument rootArguments;
				rootArguments.SRV_Vertices = blasGeometry.SRV_VertexBuffer.GetGpuHandle();
				rootArguments.SRV_Indices = blasGeometry.SRV_IndexBuffer.GetGpuHandle();
				rootArguments.SRV_DiffuseTexture = blasGeometry.SRV_Diffuse.GetGpuHandle();
				rootArguments.SRV_NormalTexture = blasGeometry.SRV_Normal.GetGpuHandle();
				rootArguments.SRV_MetalicTexture = blasGeometry.SRV_Metallic.GetGpuHandle();
				rootArguments.SRV_RoughnessTexture = blasGeometry.SRV_Roughness.GetGpuHandle();

				hitGroupShaderTable.push_back(Ideal::DXRShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));

			}
		}
		if (DeferredDeleteManager)
		{
			DeferredDeleteManager->AddD3D12ResourceToDelete(m_hitGroupShaderTable);
		}
		m_hitGroupShaderTable = hitGroupShaderTable.GetResource();
		m_hitGroupShaderTableStrideInBytes = hitGroupShaderTable.GetShaderRecordSize();
	}
}

void Ideal::RaytracingManager::CreateAnimationRootSignature(ComPtr<ID3D12Device5> Device)
{
	CD3DX12_DESCRIPTOR_RANGE ranges[AnimationRootSignature::Slot::Count];
	ranges[AnimationRootSignature::Slot::SRV_Vertices].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	// t0 : Vertices
	ranges[AnimationRootSignature::Slot::CBV_BoneData].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : Animation
	ranges[AnimationRootSignature::Slot::CBV_VertexCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);	// b1 : CBV_VertexCount
	ranges[AnimationRootSignature::Slot::UAV_OutputVertices].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);	// u0 : Output

	// binding
	CD3DX12_ROOT_PARAMETER rootParameters[AnimationRootSignature::Slot::Count];
	rootParameters[AnimationRootSignature::Slot::SRV_Vertices].InitAsDescriptorTable(1, &ranges[AnimationRootSignature::Slot::SRV_Vertices]);
	rootParameters[AnimationRootSignature::Slot::CBV_BoneData].InitAsDescriptorTable(1, &ranges[AnimationRootSignature::Slot::CBV_BoneData]);
	rootParameters[AnimationRootSignature::Slot::CBV_VertexCount].InitAsDescriptorTable(1, &ranges[AnimationRootSignature::Slot::CBV_VertexCount]);
	rootParameters[AnimationRootSignature::Slot::UAV_OutputVertices].InitAsDescriptorTable(1, &ranges[AnimationRootSignature::Slot::UAV_OutputVertices]);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
		ARRAYSIZE(rootParameters), rootParameters, 0, nullptr
	);
	SerializeAndCreateRootSignature(Device, rootSignatureDesc, &m_animationRootSignature, L"Animation Root Signature");
}

void Ideal::RaytracingManager::CreateAnimationCSPipelineState(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::D3D12Shader> AnimationShader)
{
	// TODO : 쉐이더 정보를 넘겨주어야 한다.

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
	computePipelineStateDesc.pRootSignature = m_animationRootSignature.Get();
	CD3DX12_SHADER_BYTECODE shader(AnimationShader->GetBufferPointer(), AnimationShader->GetSize());
	computePipelineStateDesc.CS = shader;
	//computePipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	Check(
		Device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_animationPipelineState)),
		L"Failed to create Animation Compute Pipeline State"
	);
}

void Ideal::RaytracingManager::DispatchAnimationComputeShader(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, uint32 CurrentContextIndex, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, std::shared_ptr<Ideal::D3D12VertexBuffer> VertexBuffer, CB_Bone* BoneData, std::shared_ptr<Ideal::D3D12UnorderedAccessView> UAV_OutVertex)
{
	CommandList->SetComputeRootSignature(m_animationRootSignature.Get());
	CommandList->SetPipelineState(m_animationPipelineState.Get());

	CommandList->SetDescriptorHeaps(1, DescriptorManager->GetDescriptorHeap().GetAddressOf());
	// Parameter0 : SRV_Vertices
	auto handle0 = DescriptorManager->Allocate(CurrentContextIndex);
	auto vertexSRV = ResourceManager->CreateSRV(VertexBuffer, VertexBuffer->GetElementCount(), VertexBuffer->GetElementSize());
	Device->CopyDescriptorsSimple(1, handle0.GetCpuHandle(), vertexSRV->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(AnimationRootSignature::Slot::SRV_Vertices, handle0.GetGpuHandle());

	// Parameter1 : CBV_BoneData
	auto handle1 = DescriptorManager->Allocate(CurrentContextIndex);
	auto cb = CBPool->Allocate(Device.Get(), sizeof(CB_Bone));
	memcpy(cb->SystemMemAddr, BoneData, sizeof(CB_Bone));
	Device->CopyDescriptorsSimple(1, handle1.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(AnimationRootSignature::Slot::CBV_BoneData, handle1.GetGpuHandle());

	// Parameter2 : CBV_VertexCount
	auto handle2 = DescriptorManager->Allocate(CurrentContextIndex);
	auto cb2 = CBPool->Allocate(Device.Get(), sizeof(uint32));
	uint32 vertexCount = VertexBuffer->GetElementCount();
	memcpy(cb2->SystemMemAddr, &vertexCount, sizeof(uint32));
	Device->CopyDescriptorsSimple(1, handle2.GetCpuHandle(), cb2->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(AnimationRootSignature::Slot::CBV_VertexCount, handle2.GetGpuHandle());

	// Parameter3 : UAV_OutputVertices
	// TEMP : 여기서 임시로 UAV까지 만든다.
	auto handle3 = DescriptorManager->Allocate(CurrentContextIndex);
	Device->CopyDescriptorsSimple(1, handle3.GetCpuHandle(), UAV_OutVertex->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(AnimationRootSignature::Slot::UAV_OutputVertices, handle3.GetGpuHandle());

	// Barrier
	CD3DX12_RESOURCE_BARRIER barrier0 = CD3DX12_RESOURCE_BARRIER::Transition(
		UAV_OutVertex->GetResource().Get(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	CommandList->ResourceBarrier(1, &barrier0);

	// DISPATCH
	uint32 elementCount = VertexBuffer->GetElementCount();
	uint32 threadsPreGroup = 1024;
	uint32 dispatchX = (elementCount + threadsPreGroup - 1) / threadsPreGroup;
	CommandList->Dispatch(dispatchX, 1, 1);

	// Barrier
	CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(UAV_OutVertex->GetResource().Get());
	CommandList->ResourceBarrier(1, &uavBarrier);


	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		UAV_OutVertex->GetResource().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	CommandList->ResourceBarrier(1, &barrier);
}