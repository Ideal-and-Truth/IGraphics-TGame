#include "IdealDynamicMeshObject.h"
#include "GraphicsEngine/Resource/Refactor/IdealAnimation.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"

Ideal::IdealDynamicMeshObject::IdealDynamicMeshObject()
{

}

Ideal::IdealDynamicMeshObject::~IdealDynamicMeshObject()
{

}

void Ideal::IdealDynamicMeshObject::Init(std::shared_ptr<IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);

	ID3D12Device* device = d3d12Renderer->GetDevice().Get();
	const uint32 bufferSize = sizeof(Transform);

	m_constantBuffer.Create(device, bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
}

void Ideal::IdealDynamicMeshObject::Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	m_dynamicMesh->Render(Renderer);
}

void Ideal::IdealDynamicMeshObject::AddAnimation(std::shared_ptr<Ideal::IAnimation> Animation)
{
	m_animations.push_back(std::static_pointer_cast<Ideal::IdealAnimation>(Animation));
}