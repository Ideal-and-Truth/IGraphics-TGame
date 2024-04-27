#pragma once
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
// Resource Head
#include "GraphicsEngine/Resource/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"

namespace Ideal
{
	template <typename TVertexType>
	class IdealMesh : public ResourceBase
	{
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
		}

		const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView()	const { return m_vertexBuffer->GetView(); }
		const D3D12_INDEX_BUFFER_VIEW&	GetIndexBufferView()	const { return m_indexBuffer->GetView(); }

		void AddVertices(const std::vector<TVertexType>& vertices)
		{
			m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
		}
		void AddIndices(const std::vector<uint32>& indices)
		{
			m_indices.insert(m_indices.end(), indices.begin(), indices.end());
		}

	private:
		std::shared_ptr<D3D12VertexBuffer>	m_vertexBuffer;
		std::shared_ptr<D3D12IndexBuffer>	m_indexBuffer;
		
		std::vector<TVertexType>			m_vertices;
		std::vector<uint32>					m_indices;
	};
}