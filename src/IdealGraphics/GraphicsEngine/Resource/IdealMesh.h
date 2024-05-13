#pragma once
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
// Resource Head
#include "GraphicsEngine/Resource/IdealBone.h"
#include "GraphicsEngine/Resource/IdealMaterial.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"

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
		IdealMesh() 
			: m_vertexBuffer(nullptr),
			m_indexBuffer(nullptr),
			m_material(nullptr),
			m_boneIndex(0)
		{}
		virtual ~IdealMesh() {};

	public:
		void Create(std::shared_ptr<D3D12Renderer> Renderer)
		{
			std::shared_ptr<Ideal::ResourceManager> resourceManager = Renderer->GetResourceManager();
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

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return m_vertexBuffer->GetView(); }
		D3D12_INDEX_BUFFER_VIEW	GetIndexBufferView() { return m_indexBuffer->GetView(); }

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

	public:
		Ideal::D3D12ConstantBuffer m_constantBuffer;
	};
}