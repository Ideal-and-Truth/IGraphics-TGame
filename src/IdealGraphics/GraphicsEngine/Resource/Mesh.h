#pragma once
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"

#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

class D3D12Renderer;
class AssimpLoader;

namespace Ideal
{
	class Model;
	class Material;
}

namespace Ideal
{
	class Mesh : public ResourceBase
	{
		friend class AssimpLoader;
		friend class Ideal::Model;
		friend class MeshObject;

	public:
		Mesh();
		virtual ~Mesh();

	public:
		void Create(std::shared_ptr<D3D12Renderer> Renderer);

		// Temp
		void Tick(std::shared_ptr<D3D12Renderer> Renderer);
		void Render(std::shared_ptr<D3D12Renderer> Renderer);

	public:
		void SetTransformMatrix(const Matrix& transform);

	private:
		void InitRootSignature(std::shared_ptr<D3D12Renderer> Renderer);
		void InitPipelineState(std::shared_ptr<D3D12Renderer> Renderer);

		void InitRootSignature2(std::shared_ptr<D3D12Renderer> Renderer);
		void InitPipelineState2(std::shared_ptr<D3D12Renderer> Renderer);

	private:
		//std::shared_ptr<IdealRenderer> m_renderer;

		ComPtr<ID3D12PipelineState> m_pipelineState;
		ComPtr<ID3D12RootSignature> m_rootSignature;

		//D3D12VertexBuffer m_vertexBuffer;
		std::shared_ptr<D3D12VertexBuffer> m_vertexBuffer;
		//D3D12IndexBuffer m_indexBuffer;
		std::shared_ptr<D3D12IndexBuffer> m_indexBuffer;
		D3D12ConstantBuffer m_constantBuffer;

		Transform m_transform;
		Transform* m_cbDataBegin;

	public:
		void AddVertices(const std::vector<BasicVertex>& vertices);
		void AddIndices(const std::vector<uint32>& indices);

	private:
		std::shared_ptr<Ideal::Material> m_material;

		int32 m_boneIndex;	// 뼈일 경우 인덱스 값
		std::string m_materialName;	// 이 mesh를 그릴때 필요한 material의 이름을 가지고 있겠다.

		std::vector<BasicVertex> m_vertices;	// 나중에 가지고 있을 필요가 없다
		std::vector<uint32> m_indices;	// 마찬가지

		// TODO : Delete
		std::wstring m_diffuseMap;

		private:
		uint32 m_descriptorIncrementSize = 0;
	};
}