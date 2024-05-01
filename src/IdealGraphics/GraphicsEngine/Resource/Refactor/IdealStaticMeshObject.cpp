#include "GraphicsEngine/Resource/Refactor/IdealStaticMeshObject.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/Resource/Refactor/IdealMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealMaterial.h"
#include "GraphicsEngine/Resource/Refactor/IdealStaticMesh.h"

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

	CB_Transform* t = (CB_Transform*)m_cbTransform.GetMappedMemory(d3d12Renderer->GetFrameIndex());
	t->World = m_transform;
	t->View = d3d12Renderer->GetView();
	t->Proj = d3d12Renderer->GetProj();
	t->WorldInvTranspose = t->World.Invert();

	commandList->SetGraphicsRootConstantBufferView(STATIC_MESH_ROOT_CONSTANT_INDEX, m_cbTransform.GetGPUVirtualAddress(d3d12Renderer->GetFrameIndex()));
	m_staticMesh->Render(Renderer);
}