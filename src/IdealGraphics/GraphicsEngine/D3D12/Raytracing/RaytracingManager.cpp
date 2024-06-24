#include "RaytracingManager.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructureManager.h"
#include "GraphicsEngine/D3D12/D3D12UploadBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12SRV.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorManager.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"

#include <d3d12.h>
#include <d3dx12.h>

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

void Ideal::RaytracingManager::Init(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12Shader> Shader, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, uint32 Width, uint32 Height)
{
	m_width = Width;
	m_height = Height;
	m_ASManager = std::make_unique<DXRAccelerationStructureManager>();
	m_uavSingleDescriptorHeap = std::make_shared<Ideal::D3D12DescriptorHeap>();
	m_uavSingleDescriptorHeap->Create(Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);
	
	CreateUAVRenderTarget(Device, Width, Height);	//device, width, height
	CreateRootSignature(Device);	//device
	CreateRaytracingPipelineStateObject(Device, Shader); // device, shader
	BuildShaderTables(Device, ResourceManager, DescriptorManager); // Device, ResourceManager
}

void Ideal::RaytracingManager::DispatchRays(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, uint32 CurrentFrameIndex, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, SceneConstantBuffer SceneCB)
{
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
	SceneConstantBuffer s;
	s.CameraPos = Vector4(0.f);
	auto handle2 = DescriptorManager->Allocate(CurrentFrameIndex);
	Device->CopyDescriptorsSimple(1, handle2.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CommandList->SetComputeRootDescriptorTable(Ideal::GlobalRootSignature::Slot::CBV_Global, handle2.GetGpuHandle());

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

	CommandList->SetPipelineState1(m_dxrStateObject.Get());
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

uint32 Ideal::RaytracingManager::AddBLASAndGetInstanceIndex(ComPtr<ID3D12Device5> Device, std::vector<BLASGeometry>& Geometries, const wchar_t* Name)
{
	// 예상으로는 중간에 만들어줘도 BLAS 자체에 IsDirty 초기값이 True 이기 때문에 Update할때 빌드가 될 것이다...
	// 일단 BLAS를 추가하고 만약 있으면 그것에 대한 인덱스만 가져온다.
	m_ASManager->AddBLAS(Device.Get(), Geometries, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE, Name);
	uint32 instanceIndex = m_ASManager->AddInstance(Name, m_contributionToHitGroupIndexCount);

	// 현재 BLAS안에 들어있는 Geometry의 개수만큼 contributionToHitGroupIndex를 늘려준다.
	uint64 geometrySizeInBLAS = Geometries.size();
	m_contributionToHitGroupIndexCount += geometrySizeInBLAS;

	return instanceIndex;
}

void Ideal::RaytracingManager::SetGeometryTransformByIndex(uint32 InstanceIndex, const Matrix& Transform)
{
	Ideal::DXRInstanceDesc* instance = m_ASManager->GetInstanceByIndex(InstanceIndex);
	instance->SetTransform(Transform);
}

void Ideal::RaytracingManager::FinalCreate(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool)
{
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	m_ASManager->InitTLAS(Device.Get(), buildFlags, true, L"RaytracingManager TLAS");
	m_ASManager->Build(CommandList, UploadBufferPool, true);
}

void Ideal::RaytracingManager::UpdateAccelerationStructures(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool)
{
	m_ASManager->Build(CommandList, UploadBufferPool);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Ideal::RaytracingManager::GetTLASResource()
{
	return m_ASManager->GetTLASResource();
}

void Ideal::RaytracingManager::CreateRootSignature(ComPtr<ID3D12Device5> Device)
{
	using namespace Ideal;
	//-------------Global Root Signature--------------//
	{
		CD3DX12_DESCRIPTOR_RANGE ranges[GlobalRootSignature::Slot::Count];
		ranges[GlobalRootSignature::Slot::UAV_Output].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);	// u0 : Output
		ranges[GlobalRootSignature::Slot::CBV_Global].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// b0 : Global

		// binding
		CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignature::Slot::Count];
		rootParameters[GlobalRootSignature::Slot::UAV_Output].InitAsDescriptorTable(1, &ranges[GlobalRootSignature::Slot::UAV_Output]);
		rootParameters[GlobalRootSignature::Slot::CBV_Global].InitAsDescriptorTable(1, &ranges[GlobalRootSignature::Slot::CBV_Global]);

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

		CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignature::Slot::Count];
		rootParameters[LocalRootSignature::Slot::SRV_IndexBuffer].InitAsDescriptorTable(1, &ranges[LocalRootSignature::Slot::SRV_IndexBuffer]);
		rootParameters[LocalRootSignature::Slot::SRV_VertexBuffer].InitAsDescriptorTable(1, &ranges[LocalRootSignature::Slot::SRV_VertexBuffer]);
		rootParameters[LocalRootSignature::Slot::SRV_Diffuse].InitAsDescriptorTable(1, &ranges[LocalRootSignature::Slot::SRV_Diffuse]);

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

void Ideal::RaytracingManager::BuildShaderTables(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager)
{
	// Reset
	DescriptorManager->ResetFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);

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
		m_rayGenShaderTable = rayGenShaderTable.GetResource();
	}

	// Miss Shader Table
	{
		uint32 numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize;
		Ideal::DXRShaderTable missShaderTable(Device.Get(), numShaderRecords, shaderRecordSize, L"MissShaderTable");
		missShaderTable.push_back(Ideal::DXRShaderRecord(missShaderIdentifier, shaderIdentifierSize));
		m_missShaderTable = missShaderTable.GetResource();
		m_missShaderTableStrideInBytes = missShaderTable.GetShaderRecordSize();
	}

	// Hit Shader Table
	{
		uint32 numShaderRecords = (uint32)m_contributionToHitGroupIndexCount;
		if (numShaderRecords == 0) numShaderRecords = 1;
		uint32 shaderRecordSize = shaderIdentifierSize + sizeof(Ideal::LocalRootSignature::RootArgument);
		Ideal::DXRShaderTable hitGroupShaderTable(Device.Get(), numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");

		const std::map<std::wstring, std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure>> BlASes = m_ASManager->GetBLASes();
		const std::vector<Ideal::BLASInstanceDesc> BlasInstances = m_ASManager->GetBLASInstanceDescs();
		for(auto& blasInstance : BlasInstances)
		{
			auto& blas = blasInstance.BLAS;
			const std::vector<BLASGeometry> blasGeometries = blas->GetGeometries();
			for (const BLASGeometry& blasGeometry : blasGeometries)
			{
				Ideal::LocalRootSignature::RootArgument rootArguments;

				std::shared_ptr<Ideal::D3D12IndexBuffer> indexBuffer = blasGeometry.IndexBuffer;
				std::shared_ptr<Ideal::D3D12VertexBuffer> vertexBuffer = blasGeometry.VertexBuffer;

				auto h1 = ResourceManager->CreateSRV(indexBuffer, indexBuffer->GetElementCount(), indexBuffer->GetElementSize());
				auto h2 = ResourceManager->CreateSRV(vertexBuffer, vertexBuffer->GetElementCount(), vertexBuffer->GetElementSize());
				
				// Indices
				auto indicesLocation = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
				Device->CopyDescriptorsSimple(1, indicesLocation.GetCpuHandle(), h1->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				
				// vertices
				auto verticesLocation = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
				Device->CopyDescriptorsSimple(1, verticesLocation.GetCpuHandle(), h2->GetHandle().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				// diffuse
				auto textureLocation = DescriptorManager->AllocateFixed(FIXED_DESCRIPTOR_HEAP_CBV_SRV_UAV);
				uint64 notExist = (uint64)D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
				if (blasGeometry.DiffuseTexture.GetCpuHandle().ptr != notExist)
				{
					Device->CopyDescriptorsSimple(1, textureLocation.GetCpuHandle(), blasGeometry.DiffuseTexture.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				rootArguments.SRV_Indices = indicesLocation.GetGpuHandle();
				rootArguments.SRV_Vertices = verticesLocation.GetGpuHandle();
				rootArguments.SRV_DiffuseTexture = textureLocation.GetGpuHandle();

				hitGroupShaderTable.push_back(Ideal::DXRShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));
			}
		}
		m_hitGroupShaderTable = hitGroupShaderTable.GetResource();
		m_hitGroupShaderTableStrideInBytes = hitGroupShaderTable.GetShaderRecordSize();
	}
}