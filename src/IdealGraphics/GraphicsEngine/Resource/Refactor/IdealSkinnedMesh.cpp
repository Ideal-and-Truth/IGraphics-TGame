#include "IdealSkinnedMesh.h"

#include "GraphicsEngine/Resource/Refactor/IdealMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealBone.h"

Ideal::IdealSkinnedMesh::IdealSkinnedMesh()
{

}

Ideal::IdealSkinnedMesh::~IdealSkinnedMesh()
{

}

void Ideal::IdealSkinnedMesh::Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();

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

void Ideal::IdealSkinnedMesh::AddMesh(std::shared_ptr<Ideal::IdealMesh<SkinnedVertex>> Mesh)
{
	m_meshes.push_back(Mesh);
}

void Ideal::IdealSkinnedMesh::AddMaterial(std::shared_ptr<Ideal::IdealMaterial> Material)
{
	for (auto& mesh : m_meshes)
	{
		// �̹� material�� ���ε� �Ǿ� ���� ���
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

void Ideal::IdealSkinnedMesh::FinalCreate(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ID3D12Device* device = d3d12Renderer->GetDevice().Get();

	for (auto& mesh : m_meshes)
	{
		mesh->Create(d3d12Renderer);
	}
	{
		const uint32 bufferSize = sizeof(CB_Transform);
		//m_cbTransform.Create(device, bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
	}
	{
		const uint32 bufferSize = sizeof(CB_Bone);
		//m_cbBoneTransform.Create(device, bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
	}
}
