#include "IdealStaticMesh.h"

#include "GraphicsEngine/Resource/Refactor/IdealMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealBone.h"

Ideal::IdealStaticMesh::IdealStaticMesh()
	:m_transform(Matrix::Identity)
{

}

Ideal::IdealStaticMesh::~IdealStaticMesh()
{

}

void Ideal::IdealStaticMesh::Render(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();

	Transform* t = (Transform*)m_constantBuffer.GetMappedMemory(d3d12Renderer->GetFrameIndex());
	t->World = m_transform;
	t->View = d3d12Renderer->GetView();
	t->Proj = d3d12Renderer->GetProj();

	commandList->SetGraphicsRootConstantBufferView(STATIC_MESH_ROOT_CONSTANT_INDEX, m_constantBuffer.GetGPUVirtualAddress(d3d12Renderer->GetFrameIndex()));
	
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

void Ideal::IdealStaticMesh::SetTransformMatrix(const Matrix& Transform)
{
	m_transform = Transform;
}

void Ideal::IdealStaticMesh::AddMesh(std::shared_ptr<Ideal::IdealMesh<BasicVertex>> Mesh)
{
	m_meshes.push_back(Mesh);
}

void Ideal::IdealStaticMesh::AddMaterial(std::shared_ptr<Ideal::IdealMaterial> Material)
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

void Ideal::IdealStaticMesh::FinalCreate(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	std::shared_ptr<D3D12Renderer> d3d12Renderer = std::static_pointer_cast<D3D12Renderer>(Renderer);
	for (auto& mesh : m_meshes)
	{
		mesh->Create(d3d12Renderer);
	}

	const uint32 bufferSize = sizeof(Transform);
	m_constantBuffer.Create(d3d12Renderer->GetDevice().Get(), bufferSize, D3D12Renderer::FRAME_BUFFER_COUNT);
}
