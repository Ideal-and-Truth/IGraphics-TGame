#include "GraphicsEngine/Resource/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/IdealMesh.h"
#include "GraphicsEngine/Resource/IdealMaterial.h"
#include "GraphicsEngine/Resource/IdealStaticMesh.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/Raytracing/RaytracingManager.h"
#include "Misc/Utils/StringUtils.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructureManager.h"

Ideal::IdealStaticMeshObject::IdealStaticMeshObject()
{
}

Ideal::IdealStaticMeshObject::~IdealStaticMeshObject()
{

}

uint32 Ideal::IdealStaticMeshObject::GetMeshesSize()
{
	uint32 ret = m_staticMesh->GetMeshes().size();
	return ret;
}

std::weak_ptr<Ideal::IMesh> Ideal::IdealStaticMeshObject::GetMeshByIndex(uint32 index)
{
	if (index >= m_staticMesh->GetMeshes().size())
	{
		return std::weak_ptr<Ideal::IMesh>();
	}
	auto ret = m_staticMesh->GetMeshes()[index];
	return ret;
}

uint32 Ideal::IdealStaticMeshObject::GetBonesSize()
{
	return 0;
}

std::weak_ptr<Ideal::IBone> Ideal::IdealStaticMeshObject::GetBoneByIndex(uint32 index)
{
	return std::weak_ptr<Ideal::IBone>();
}

void Ideal::IdealStaticMeshObject::Init(ComPtr<ID3D12Device> Device)
{

}

void Ideal::IdealStaticMeshObject::Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	if (!m_isDraw)
	{
		return;
	}
	// Ver2 2024.05.07 : Constant Buffer를 Pool에서 할당받아 사용한다.
// 	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
// 	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
// 	ComPtr<ID3D12Device> device = d3d12Renderer->GetDevice();

// 	std::shared_ptr<Ideal::D3D12DescriptorHeap> descriptorHeap = d3d12Renderer->GetMainDescriptorHeap();
// 	auto cb = d3d12Renderer->ConstantBufferAllocate(sizeof(CB_Transform));
// 	if (!cb)
// 	{
// 		__debugbreak();
// 	}
// 
// 	CB_Transform* cbTransform = (CB_Transform*)cb->SystemMemAddr;
// 	cbTransform->World = m_transform.Transpose();
// 	cbTransform->WorldInvTranspose = m_transform.Transpose().Invert();
// 
// 	// b0용 Descriptor Table 할당
// 	auto handle = descriptorHeap->Allocate(1);	// 공용 root parameter인 b0 : transform 만 필요하니 Descriptor 공간을 하나만 받는다.
// 	uint32 incrementSize = d3d12Renderer->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
// 
// 	// Copy To Main Descriptor
// 	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_CBV_TRANSFORM, incrementSize);
// 	device->CopyDescriptorsSimple(1, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
// 
// 	// 공용으로 쓰는 Root Parameter인 Transform은 Root Paramter Index가 0번임.
// 	commandList->SetGraphicsRootDescriptorTable(STATIC_MESH_DESCRIPTOR_TABLE_INDEX_OBJ, handle.GetGpuHandle());

	m_staticMesh->Draw(Renderer, m_transform);
}

void Ideal::IdealStaticMeshObject::UpdateBLASInstance(std::shared_ptr<Ideal::RaytracingManager> RaytracingManager)
{
	if (m_isDirty)
	{
		//RaytracingManager->SetGeometryTransformByIndex(m_instanceIndex, m_transform);
		m_BLASInstanceDesc->InstanceDesc.SetTransform(m_transform);
		//m_blas->SetDirty(true);
		m_isDirty = false;
	}
}

void Ideal::IdealStaticMeshObject::SetBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> InBLAS)
{
	m_blas = InBLAS;
}

std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> Ideal::IdealStaticMeshObject::GetBLAS()
{
	return m_blas;
}
