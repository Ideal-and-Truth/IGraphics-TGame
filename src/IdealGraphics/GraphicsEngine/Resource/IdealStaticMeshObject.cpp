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
Ideal::IdealStaticMeshObject::IdealStaticMeshObject()
{

}

Ideal::IdealStaticMeshObject::~IdealStaticMeshObject()
{

}

void Ideal::IdealStaticMeshObject::Init(ComPtr<ID3D12Device> Device)
{
	//std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Device);
	//ID3D12Device* device = d3d12Renderer->GetDevice().Get();
}

void Ideal::IdealStaticMeshObject::Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	if (!m_isDraw)
	{
		return;
	}
	// Ver2 2024.05.07 : Constant Buffer를 Pool에서 할당받아 사용한다.
	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	ComPtr<ID3D12Device> device = d3d12Renderer->GetDevice();

	std::shared_ptr<Ideal::D3D12DescriptorHeap> descriptorHeap = d3d12Renderer->GetMainDescriptorHeap();
	auto cb = d3d12Renderer->ConstantBufferAllocate(sizeof(CB_Transform));
	if (!cb)
	{
		__debugbreak();
	}

	CB_Transform* cbTransform = (CB_Transform*)cb->SystemMemAddr;
	cbTransform->World = m_transform.Transpose();
	cbTransform->WorldInvTranspose = m_transform.Transpose().Invert();

	// b0용 Descriptor Table 할당
	auto handle = descriptorHeap->Allocate(1);	// 공용 root parameter인 b0 : transform 만 필요하니 Descriptor 공간을 하나만 받는다.
	uint32 incrementSize = d3d12Renderer->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Copy To Main Descriptor
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_CBV_TRANSFORM, incrementSize);
	device->CopyDescriptorsSimple(1, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 공용으로 쓰는 Root Parameter인 Transform은 Root Paramter Index가 0번임.
	commandList->SetGraphicsRootDescriptorTable(STATIC_MESH_DESCRIPTOR_TABLE_INDEX_OBJ, handle.GetGpuHandle());

	m_staticMesh->Draw(Renderer);
}

void Ideal::IdealStaticMeshObject::AllocateBLASInstanceID(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::RaytracingManager> RaytracingManager)
{
	// 일단 geometry들을 받아온다.
	auto& geometries = m_staticMesh->GetMeshes();

	uint64 numMesh = geometries.size();

	Ideal::BLASData blasGeometryDesc;
	for (uint32 i = 0; i < numMesh; ++i)
	{
		Ideal::BLASGeometry blasGeometry;
		blasGeometry.Name = L"";	// temp
		blasGeometry.VertexBuffer = geometries[i]->GetVertexBuffer();
		blasGeometry.IndexBuffer = geometries[i]->GetIndexBuffer();

		std::shared_ptr<Ideal::IdealMaterial> material = geometries[i]->GetMaterial();
		if (material)
		{
			std::shared_ptr<Ideal::D3D12Texture> diffuseTexture = material->GetDiffuseTexture();
			if (diffuseTexture)
			{
				blasGeometry.DiffuseTexture = diffuseTexture->GetSRV();
			}
		}

		blasGeometryDesc.Geometries.push_back(blasGeometry);
	}
	const std::wstring& name = GetName();
	m_instanceID = RaytracingManager->AddBLASAndGetInstanceIndex(Device, blasGeometryDesc.Geometries, name.c_str());
}

void Ideal::IdealStaticMeshObject::UpdateBLASInstance(std::shared_ptr<Ideal::RaytracingManager> RaytracingManager)
{
	RaytracingManager->SetGeometryTransformByIndex(m_instanceID, m_transform);
}
