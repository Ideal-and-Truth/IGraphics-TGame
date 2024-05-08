#include "GraphicsEngine/Resource/Refactor/IdealStaticMeshObject.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/Resource/Refactor/IdealMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealMaterial.h"
#include "GraphicsEngine/Resource/Refactor/IdealStaticMesh.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"

Ideal::IdealStaticMeshObject::IdealStaticMeshObject()
{

}

Ideal::IdealStaticMeshObject::~IdealStaticMeshObject()
{

}

void Ideal::IdealStaticMeshObject::Init(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);

	ID3D12Device* device = d3d12Renderer->GetDevice().Get();
	{
		const uint32 bufferSize = sizeof(CB_Transform);
		m_cbTransform.Create(device, bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
	}
}

void Ideal::IdealStaticMeshObject::Draw(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();

	// Ver1
	CB_Transform* t = (CB_Transform*)m_cbTransform.GetMappedMemory(d3d12Renderer->GetFrameIndex());
	t->World = m_transform;
	t->View = d3d12Renderer->GetView();
	t->Proj = d3d12Renderer->GetProj();
	t->WorldInvTranspose = t->World.Invert();

	commandList->SetGraphicsRootConstantBufferView(STATIC_MESH_ROOT_CONSTANT_INDEX_TRANSFORM, m_cbTransform.GetGPUVirtualAddress(d3d12Renderer->GetFrameIndex()));
	m_staticMesh->Draw(Renderer);
}

void Ideal::IdealStaticMeshObject::Draw2(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	// Ver2 2024.05.07 : Constant Buffer를 Pool에서 할당받아 사용한다.
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	ComPtr<ID3D12Device> device = d3d12Renderer->GetDevice();

	std::shared_ptr<Ideal::D3D12DescriptorHeap> descriptorHeap = d3d12Renderer->GetMainDescriptorHeap();
	std::shared_ptr<Ideal::D3D12ConstantBufferPool> cbPool = d3d12Renderer->GetCBPool(sizeof(CB_Transform));

	auto cb = cbPool->Allocate();
	if (!cb)
	{
		__debugbreak();
	}

	CB_Transform* cbTransform = (CB_Transform*)cb->SystemMemAddr;
	cbTransform->World = m_transform;
	cbTransform->View = d3d12Renderer->GetView();
	cbTransform->Proj = d3d12Renderer->GetProj();
	cbTransform->WorldInvTranspose = m_transform.Invert();

	// b0용 Descriptor Table 할당
	auto handle = descriptorHeap->Allocate(1);	// 공용 root parameter인 b0 : transform 만 필요하니 Descriptor 공간을 하나만 받는다.
	uint32 incrementSize = d3d12Renderer->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Copy To Main Descriptor
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(handle.GetCpuHandle(), STATIC_MESH_DESCRIPTOR_INDEX_CBV_TRANSFORM, incrementSize);
	device->CopyDescriptorsSimple(1, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 공용으로 쓰는 Root Parameter인 Transform은 Root Paramter Index가 0번임.
	commandList->SetGraphicsRootDescriptorTable(STATIC_MESH_DESCRIPTOR_TABLE_INDEX_OBJ, handle.GetGpuHandle());

	m_staticMesh->Draw2(Renderer);
}
