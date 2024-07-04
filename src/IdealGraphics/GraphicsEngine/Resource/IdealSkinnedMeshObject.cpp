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
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructureManager.h"

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

	// 현재 애니메이션이 끝났는데 다음 애니메이션이 있을 경우?
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

				// 마지막 프레임의 애니메이션이 끝났다는 것
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
			// next animation이 없을 경우 다시 재생
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
				// 현재 프레임 + 1이 현재 애니메이션의 최대 프레임 - 1 보다 클 경우 애니메이션은 끝났다고 처리한다.
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

void Ideal::IdealSkinnedMeshObject::CreateUAVVertexBuffer(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager)
{
	auto& meshes = m_skinnedMesh->GetMeshes();
	uint64 numMesh = meshes.size();
	// 그냥 하나라고 가정하고 씀
	uint32 numVertexCount = meshes[0]->GetVertexBuffer()->GetElementCount();
	uint32 size = numVertexCount * sizeof(BasicVertex);
	m_uavBuffer = std::make_shared<Ideal::D3D12UAVBuffer>();
	m_uavBuffer->Create(Device.Get(), size, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"UAV_SkinnedVertex");
	m_uavView = ResourceManager->CreateUAV(m_uavBuffer, numVertexCount, sizeof(BasicVertex));
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
		auto& meshes = m_skinnedMesh->GetMeshes();
		auto& geometries = m_BLAS->GetGeometries();

		uint64 numMesh = meshes.size();

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
					meshes[i]->GetVertexBuffer(),
					&m_cbBoneData,
					m_uavView
				);
			}
		}
		m_BLAS->SetDirty(true);
		m_BLASInstanceDesc->InstanceDesc.SetTransform(m_transform);
		//RaytracingManager->SetGeometryTransformByIndex(m_instanceIndex, m_transform);

		m_isDirty = false;
	}
}

void Ideal::IdealSkinnedMeshObject::SetBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> InBLAS)
{
	m_BLAS = InBLAS;
}

std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> Ideal::IdealSkinnedMeshObject::GetBLAS()
{
	return m_BLAS;
}

void Ideal::IdealSkinnedMeshObject::SetBLASInstanceIndex(uint32 InstanceIndex)
{
	m_instanceIndex = InstanceIndex;
}