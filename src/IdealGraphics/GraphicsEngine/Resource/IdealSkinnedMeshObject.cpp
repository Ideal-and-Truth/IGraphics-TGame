#include "Core/Core.h"
#include "IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/IdealMesh.h"
#include "GraphicsEngine/Resource/IdealMaterial.h"
#include "GraphicsEngine/Resource/IdealBone.h"
#include "GraphicsEngine/Resource/IdealAnimation.h"
#include "GraphicsEngine/Resource/IdealSkinnedMesh.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/Raytracing/RaytracingManager.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorManager.h"
#include "GraphicsEngine/D3D12/D3D12UAV.h"

#include <d3dx12.h>
//#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"

Ideal::IdealSkinnedMeshObject::IdealSkinnedMeshObject()
{

}

Ideal::IdealSkinnedMeshObject::~IdealSkinnedMeshObject()
{

}

void Ideal::IdealSkinnedMeshObject::Init(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ID3D12Device* device = d3d12Renderer->GetDevice().Get();
}

void Ideal::IdealSkinnedMeshObject::Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	if (!m_isDraw)
	{
		return;
	}

	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	ComPtr<ID3D12Device> device = d3d12Renderer->GetDevice();
	std::shared_ptr<Ideal::D3D12DescriptorHeap> descriptorHeap = d3d12Renderer->GetMainDescriptorHeap();

	AnimationPlay();

	// Bind Descriptor Table
	auto handle = descriptorHeap->Allocate(2);
	uint32 incrementSize = d3d12Renderer->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	commandList->SetGraphicsRootDescriptorTable(SKINNED_MESH_DESCRIPTOR_TABLE_INDEX_OBJ, handle.GetGpuHandle());

	{
		// Bind Transform
		auto cb = d3d12Renderer->ConstantBufferAllocate(sizeof(CB_Transform));
		if (!cb)
		{
			__debugbreak();
		}

		CB_Transform* t = (CB_Transform*)cb->SystemMemAddr;
		t->World = m_transform.Transpose();
		t->WorldInvTranspose = m_transform.Transpose().Invert();

		// Copy To Main Descriptor Table
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(handle.GetCpuHandle(), SKINNED_MESH_DESCRIPTOR_INDEX_CBV_TRANSFORM, incrementSize);
		device->CopyDescriptorsSimple(1, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	// Bind Bone
	{
		auto cb = d3d12Renderer->ConstantBufferAllocate(sizeof(CB_Bone));
		if (!cb)
		{
			__debugbreak();
		}

		CB_Bone* b = (CB_Bone*)cb->SystemMemAddr;
		*b = m_cbBoneData;

		// Copy To Main Descriptor Table
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(handle.GetCpuHandle(), SKINNED_MESH_DESCRIPTOR_INDEX_CBV_BONE, incrementSize);
		device->CopyDescriptorsSimple(1, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	m_skinnedMesh->Draw(Renderer);
}

void Ideal::IdealSkinnedMeshObject::SetAnimation(const std::string& AnimationName, bool WhenCurrentAnimationFinished /*= true*/)
{
	m_whenCurrentAnimationFinishChangeAnimation = WhenCurrentAnimationFinished;
	m_nextAnimation = m_animations[AnimationName];
	if (m_nextAnimation == nullptr)
	{
		MessageBoxA(NULL, "NullException - SetAnimation", "SetAnimation", MB_OK);
		assert(false);
	}
}

void Ideal::IdealSkinnedMeshObject::AddAnimation(const std::string& AnimationName, std::shared_ptr<Ideal::IAnimation> Animation)
{
	if (m_animations[AnimationName] != nullptr)
	{
		MessageBoxA(NULL, "Already Have Same Name Animation", "AddAnimation Error", MB_OK);
		return;
	}
	m_animations[AnimationName] = std::static_pointer_cast<Ideal::IdealAnimation>(Animation);
	if (m_currentAnimation == nullptr)
	{
		m_currentAnimation = std::static_pointer_cast<Ideal::IdealAnimation>(Animation);
	}
}

void Ideal::IdealSkinnedMeshObject::SetSkinnedMesh(std::shared_ptr<Ideal::IdealSkinnedMesh> Mesh)
{
	m_skinnedMesh = Mesh;

	auto bones = Mesh->GetBones();
	for (auto b : bones)
	{
		std::shared_ptr<Ideal::IdealBone> newBone = std::make_shared<Ideal::IdealBone>();
		newBone->SetName(b->GetName());
		newBone->SetBoneIndex(b->GetBoneIndex());
		newBone->SetParent(b->GetParent());
		newBone->SetTransform(b->GetTransform());
		m_bones.push_back(newBone);
	}
}

void Ideal::IdealSkinnedMeshObject::AnimationPlay()
{
	m_sumTime += 0.001f;

	// ���� �ִϸ��̼��� �����µ� ���� �ִϸ��̼��� ���� ���?
	bool changeAnimationFlag = false;
	if (m_isAnimationFinished)
	{
		if (m_nextAnimation)
		{
			if (m_whenCurrentAnimationFinishChangeAnimation)
			{
				float timePerFrame = 1 / (m_currentAnimation->frameRate * m_animSpeed);

				for (uint32 boneIdx = 0; boneIdx < m_bones.size(); ++boneIdx)
				{
					Matrix currentFrame = m_currentAnimation->m_animTransform->transforms[m_currentAnimation->frameCount - 1][boneIdx];
					Matrix nextFrame = m_nextAnimation->m_animTransform->transforms[0][boneIdx];
					Matrix resultFrame = Matrix::Identity;
					Matrix::Lerp(currentFrame, nextFrame, m_ratio, resultFrame);
					m_cbBoneData.transforms[boneIdx] = resultFrame.Transpose();
				}
				m_ratio = m_sumTime / timePerFrame;

				// ������ �������� �ִϸ��̼��� �����ٴ� ��
				if (m_ratio >= 1.0f)
				{
					m_currentAnimation = m_nextAnimation;
					m_nextAnimation = nullptr;
					m_currentFrame = 0;
					m_nextFrame = 0;
					m_isAnimationFinished = false;
					m_whenCurrentAnimationFinishChangeAnimation = false;
				}

				changeAnimationFlag = true;
			}
			else
			{
				m_currentAnimation = m_nextAnimation;
				m_nextAnimation = nullptr;
				m_currentFrame = 0;
				m_nextFrame = 0;
				m_isAnimationFinished = false;
			}
		}
		else
		{
			// next animation�� ���� ��� �ٽ� ���
			m_isAnimationFinished = false;
		}
	}

	if (changeAnimationFlag == false && m_currentAnimation)
	{
		// Bone Setting
		{
			float timePerFrame = 1 / (m_currentAnimation->frameRate * m_animSpeed);
			if (m_sumTime >= timePerFrame)
			{
				m_sumTime = 0.f;
				// ���� ������ + 1�� ���� �ִϸ��̼��� �ִ� ������ - 1 ���� Ŭ ��� �ִϸ��̼��� �����ٰ� ó���Ѵ�.
				if (m_currentFrame + 1 > m_currentAnimation->frameCount - 1)
				{
					m_isAnimationFinished = true;
				}
				m_currentFrame = (m_currentFrame + 1) % m_currentAnimation->frameCount;
				m_nextFrame = (m_currentFrame + 1) % m_currentAnimation->frameCount;
			}

			m_ratio = m_sumTime / timePerFrame;

			for (uint32 boneIdx = 0; boneIdx < m_bones.size(); ++boneIdx)
			{
				// Current Frame
				Matrix currentFrame = m_currentAnimation->m_animTransform->transforms[m_currentFrame][boneIdx];
				Matrix nextFrame = m_currentAnimation->m_animTransform->transforms[m_nextFrame][boneIdx];
				Matrix resultFrame = Matrix::Identity;
				Matrix::Lerp(currentFrame, nextFrame, m_ratio, resultFrame);
				m_cbBoneData.transforms[boneIdx] = resultFrame.Transpose();
			}
		}
	}
}

void Ideal::IdealSkinnedMeshObject::AllocateBLASInstanceID(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::RaytracingManager> RaytracingManager, std::shared_ptr<Ideal::ResourceManager> ResourceManager)
{
	auto& geometries = m_skinnedMesh->GetMeshes();

	uint64 numMesh = geometries.size();

	Ideal::BLASData blasGeometryDesc;
	blasGeometryDesc.Geometries.resize(numMesh);
	for (uint32 i = 0; i < numMesh; ++i)
	{
		Ideal::BLASGeometry blasGeometry;
		blasGeometry.Name = L"";	// temp

		m_uavBuffer = std::make_shared<Ideal::D3D12UAVBuffer>();
		uint32 count = geometries[i]->GetVertexBuffer()->GetElementCount();
		uint32 size = count * sizeof(BasicVertex);
		m_uavBuffer->Create(Device.Get(), size, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"UAV_SkinnedVertex");
		m_uavView = ResourceManager->CreateUAV(m_uavBuffer, count, sizeof(BasicVertex));

		//blasGeometry.VertexBufferGPUAddress = geometries[i]->GetVertexBuffer()->GetResource()->GetGPUVirtualAddress();
		//blasGeometry.VertexCount = geometries[i]->GetVertexBuffer()->GetElementCount();
		//blasGeometry.VertexStrideInBytes = geometries[i]->GetVertexBuffer()->GetElementSize();
		blasGeometry.VertexBufferGPUAddress = m_uavBuffer->GetGPUVirtualAddress();
		blasGeometry.VertexCount = geometries[i]->GetVertexBuffer()->GetElementCount();
		blasGeometry.VertexStrideInBytes = sizeof(BasicVertex);
		blasGeometry.IndexBufferGPUAddress = geometries[i]->GetIndexBuffer()->GetResource()->GetGPUVirtualAddress();
		blasGeometry.IndexCount = geometries[i]->GetIndexBuffer()->GetElementCount();

		std::shared_ptr<Ideal::IdealMaterial> material = geometries[i]->GetMaterial();
		if (material)
		{
			std::shared_ptr<Ideal::D3D12Texture> diffuseTexture = material->GetDiffuseTexture();
			if (diffuseTexture)
			{
				blasGeometry.DiffuseTexture = diffuseTexture->GetSRV();
			}
		}
		blasGeometryDesc.Geometries[i] = blasGeometry;
	}
	const std::wstring& name = GetName();
	Ideal::InstanceInfo instanceInfo = RaytracingManager->AddBLASAndGetInstanceIndex(Device, blasGeometryDesc.Geometries, name.c_str(), true);

	m_instanceID = instanceInfo.InstanceIndex;
	m_BLAS = instanceInfo.BLAS;
}

void Ideal::IdealSkinnedMeshObject::UpdateBLASInstance(
	ComPtr<ID3D12Device5> Device,
	ComPtr<ID3D12GraphicsCommandList4> CommandList,
	std::shared_ptr<Ideal::ResourceManager> ResourceManager,
	std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager,
	uint32 CurrentContextIndex,
	std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool,
	std::shared_ptr<Ideal::RaytracingManager> RaytracingManager
)
{
	AnimationPlay();

	{
		auto& geometries = m_skinnedMesh->GetMeshes();

		uint64 numMesh = geometries.size();

		Ideal::BLASData blasGeometryDesc;
		blasGeometryDesc.Geometries.resize(numMesh);
		for (uint32 i = 0; i < numMesh; ++i)
		{
			// Calculate Bone Transform CS
			{
				RaytracingManager->DispatchAnimationComputeShader(
					Device,
					CommandList,
					ResourceManager,
					DescriptorManager,
					CurrentContextIndex,
					CBPool,
					geometries[i]->GetVertexBuffer(),
					&m_cbBoneData,
					m_uavView
				);
			}

			Ideal::BLASGeometry blasGeometry;
			blasGeometry.VertexBufferResource = m_uavBuffer->GetResource();
			blasGeometry.VertexBufferGPUAddress = m_uavBuffer->GetGPUVirtualAddress();
			blasGeometry.VertexCount = geometries[i]->GetVertexBuffer()->GetElementCount();
			//blasGeometry.VertexStrideInBytes = geometries[i]->GetVertexBuffer()->GetElementSize();
			blasGeometry.VertexStrideInBytes = sizeof(BasicVertex);
			blasGeometry.IndexBufferResource = geometries[i]->GetIndexBuffer()->GetResource();
			blasGeometry.IndexBufferGPUAddress = geometries[i]->GetIndexBuffer()->GetResource()->GetGPUVirtualAddress();
			blasGeometry.IndexCount = geometries[i]->GetIndexBuffer()->GetElementCount();

			std::shared_ptr<Ideal::IdealMaterial> material = geometries[i]->GetMaterial();
			if (material)
			{
				std::shared_ptr<Ideal::D3D12Texture> diffuseTexture = material->GetDiffuseTexture();
				if (diffuseTexture)
				{
					blasGeometry.DiffuseTexture = diffuseTexture->GetSRV();
				}
			}
			blasGeometryDesc.Geometries[i] = blasGeometry;
		}

		m_BLAS->BuildGeometries(blasGeometryDesc.Geometries);
		m_BLAS->SetDirty(true);

		//RaytracingManager->SetGeometryTransformByIndex(m_instanceID, m_transform);

		m_isDirty = false;
	}
}

void Ideal::IdealSkinnedMeshObject::SetBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> InBLAS)
{
	m_BLAS = InBLAS;
}

//std::vector<Ideal::BLASGeometry> Ideal::IdealSkinnedMeshObject::GetBLASGeometries(ComPtr<ID3D12Device> Device, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager)
//{
//	auto& geometries = m_skinnedMesh->GetMeshes();
//
//	uint64 numMesh = geometries.size();
//
//	Ideal::BLASData blasGeometryDesc;
//	blasGeometryDesc.Geometries.resize(numMesh);
//	for (uint32 i = 0; i < numMesh; ++i)
//	{
//		Ideal::BLASGeometry blasGeometry;
//		blasGeometry.Name = L"";	// temp
//
//		m_uavBuffer = std::make_shared<Ideal::D3D12UAVBuffer>();
//		uint32 count = geometries[i]->GetVertexBuffer()->GetElementCount();
//		uint32 size = count * sizeof(BasicVertex);
//		m_uavBuffer->Create(Device.Get(), size, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"UAV_SkinnedVertex");
//		m_uavView = ResourceManager->CreateUAV(m_uavBuffer, count, sizeof(BasicVertex));
//
//		//blasGeometry.VertexBufferGPUAddress = geometries[i]->GetVertexBuffer()->GetResource()->GetGPUVirtualAddress();
//		//blasGeometry.VertexCount = geometries[i]->GetVertexBuffer()->GetElementCount();
//		//blasGeometry.VertexStrideInBytes = geometries[i]->GetVertexBuffer()->GetElementSize();
//		blasGeometry.VertexBufferGPUAddress = m_uavBuffer->GetGPUVirtualAddress();
//		blasGeometry.VertexCount = geometries[i]->GetVertexBuffer()->GetElementCount();
//		blasGeometry.VertexStrideInBytes = sizeof(BasicVertex);
//		blasGeometry.IndexBufferGPUAddress = geometries[i]->GetIndexBuffer()->GetResource()->GetGPUVirtualAddress();
//		blasGeometry.IndexCount = geometries[i]->GetIndexBuffer()->GetElementCount();
//
//		std::shared_ptr<Ideal::IdealMaterial> material = geometries[i]->GetMaterial();
//		if (material)
//		{
//			std::shared_ptr<Ideal::D3D12Texture> diffuseTexture = material->GetDiffuseTexture();
//			if (diffuseTexture)
//			{
//				blasGeometry.DiffuseTexture = diffuseTexture->GetSRV();
//			}
//		}
//		blasGeometryDesc.Geometries[i] = blasGeometry;
//	}
//	const std::wstring& name = GetName();
//	Ideal::InstanceInfo instanceInfo = RaytracingManager->AddBLASAndGetInstanceIndex(Device, blasGeometryDesc.Geometries, name.c_str(), true);
//
//	m_instanceID = instanceInfo.InstanceIndex;
//	m_BLAS = instanceInfo.BLAS;
//}