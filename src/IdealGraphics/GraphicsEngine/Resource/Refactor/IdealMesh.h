#pragma once
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
// Resource Head
#include "GraphicsEngine/Resource/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/Resource/Refactor/IdealBone.h"
#include "GraphicsEngine/Resource/Refactor/IdealMaterial.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/D3D12ResourceManager.h"

namespace Ideal
{
	class IdealMaterial;
}

namespace Ideal
{
	template <typename TVertexType>
	class IdealMesh : public ResourceBase
	{
		//friend class IdealStaticMeshObject;

	public:
		IdealMesh() {}
		virtual ~IdealMesh() {};

	public:
		void Create(std::shared_ptr<D3D12Renderer> Renderer)
		{
			std::shared_ptr<Ideal::D3D12ResourceManager> resourceManager = Renderer->GetResourceManager();
			//-------------VB-------------//
			m_vertexBuffer = std::make_shared<Ideal::D3D12VertexBuffer>();
			resourceManager->CreateVertexBuffer<TVertexType>(m_vertexBuffer, m_vertices);

			//-------------IB-------------//
			m_indexBuffer = std::make_shared<Ideal::D3D12IndexBuffer>();
			resourceManager->CreateIndexBuffer(m_indexBuffer, m_indices);

			//------------Clear-----------//
			m_vertices.clear();
			m_indices.clear();

			//----------Material----------//
			if (m_material)
			{
				m_material->Create(Renderer);
			}
		}

		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView()	const { return m_vertexBuffer->GetView(); }
		const D3D12_INDEX_BUFFER_VIEW&	GetIndexBufferView()	const { return m_indexBuffer->GetView(); }

		const uint32& GetElementCount() const { return m_indexBuffer->GetElementCount(); }

		void AddVertices(const std::vector<TVertexType>& vertices)
		{
			m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
		}
		void AddIndices(const std::vector<uint32>& indices)
		{
			m_indices.insert(m_indices.end(), indices.begin(), indices.end());
		}

		const std::string& GetMaterialName() { return m_materialName; }
		void SetMaterialName(const std::string& MaterialName) { m_materialName = MaterialName; }
		std::shared_ptr<Ideal::IdealMaterial> GetMaterial() { return m_material; }
		void SetMaterial(std::shared_ptr<IdealMaterial> Material) { m_material = Material; }

		void SetBoneIndex(const int32& Index) { m_boneIndex = Index; }
	private:
		std::shared_ptr<D3D12VertexBuffer>	m_vertexBuffer;
		std::shared_ptr<D3D12IndexBuffer>	m_indexBuffer;
		
		std::vector<TVertexType>			m_vertices;
		std::vector<uint32>					m_indices;


		std::shared_ptr<Ideal::IdealMaterial> m_material;
		std::string m_materialName;

		std::vector<Ideal::IdealBone> m_bones;
		int32 m_boneIndex;
	};
}