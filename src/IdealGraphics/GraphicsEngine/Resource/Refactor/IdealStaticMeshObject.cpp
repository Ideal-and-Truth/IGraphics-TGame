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
	const uint32 bufferSize = sizeof(CB_Transform);

	m_constantBuffer.Create(device, bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
}

void Ideal::IdealStaticMeshObject::Draw(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	m_staticMesh->Render(Renderer);
}