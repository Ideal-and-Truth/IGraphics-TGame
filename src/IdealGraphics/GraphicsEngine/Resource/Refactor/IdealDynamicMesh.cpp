#include "IdealDynamicMesh.h"

#include "GraphicsEngine/Resource/Refactor/IdealMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealBone.h"

Ideal::IdealDynamicMesh::IdealDynamicMesh()
{

}

Ideal::IdealDynamicMesh::~IdealDynamicMesh()
{

}

void Ideal::IdealDynamicMesh::Render(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();

	CB_Transform* t = (CB_Transform*)m_cbTransform.GetMappedMemory(d3d12Renderer->GetFrameIndex());
	t->World = m_transform;
	t->View = d3d12Renderer->GetView();
	t->Proj = d3d12Renderer->GetProj();

	commandList->SetGraphicsRootConstantBufferView(DYNAMIC_MESH_ROOT_CONSTANT_INDEX, m_cbTransform.GetGPUVirtualAddress(d3d12Renderer->GetFrameIndex()));
	commandList->SetGraphicsRootConstantBufferView(DYNAMIC_MESH_ROOT_CONSTANT_INDEX + 1, m_cbBoneTransform.GetGPUVirtualAddress(d3d12Renderer->GetFrameIndex()));

	for (auto& mesh : m_meshes)
	{
		// Mesh
		const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView = mesh->GetVertexBufferView();
		const D3D12_INDEX_BUFFER_VIEW& indexBufferView = mesh->GetIndexBufferView();

		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetIndexBuffer(&indexBufferView);

		// Material
		std::shared_ptr<Ideal::IdealMaterial> material = mesh->GetMaterial();
		if (material != nullptr)
		{
			material->BindToShader(d3d12Renderer);
		}
		// Final Draw
		commandList->DrawIndexedInstanced(mesh->GetElementCount(), 1, 0, 0, 0);
	}
}

void Ideal::IdealDynamicMesh::SetTransformMatrix(const Matrix& Transform)
{
	m_transform = Transform;
}

void Ideal::IdealDynamicMesh::AddMesh(std::shared_ptr<Ideal::IdealMesh<SkinnedVertex>> Mesh)
{
	m_meshes.push_back(Mesh);
}

void Ideal::IdealDynamicMesh::AddMaterial(std::shared_ptr<Ideal::IdealMaterial> Material)
{
	for (auto& mesh : m_meshes)
	{
		// 이미 material이 바인딩 되어 있을 경우
		if (mesh->GetMaterial() != nullptr)
		{
			continue;
		}

		if (Material->GetName() == mesh->GetMaterialName())
		{
			mesh->SetMaterial(Material);
			return;
		}
	}
}

void Ideal::IdealDynamicMesh::FinalCreate(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ID3D12Device* device = d3d12Renderer->GetDevice().Get();

	for (auto& mesh : m_meshes)
	{
		mesh->Create(d3d12Renderer);
	}

	{
		const uint32 bufferSize = sizeof(CB_Transform);
		m_cbTransform.Create(device, bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
	}
	{
		const uint32 bufferSize = sizeof(CB_Bone);
		m_cbBoneTransform.Create(device, bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
	}
}
