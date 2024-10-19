#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/ConstantBufferInfo.h"
#include "GraphicsEngine/VertexInfo.h"

struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

namespace Ideal
{
	class D3D12Texture;
	class D3D12DescriptorHeap;
	class D3D12DynamicConstantBufferAllocator;
	class D3D12Viewport;
	class D3D12Shader;
	class DeferredDeleteManager;
	class ResourceManager;
	template<typename> class IdealMesh;
}

namespace Ideal
{
	// 모든 포스트 프로세싱을 관리? 또는 합치기 위한 클래스
	class CompositePass
	{
	public:
		void InitCompositePass(
			std::shared_ptr<Ideal::D3D12Shader> CompositeVS,
			std::shared_ptr<Ideal::D3D12Shader> CompositePS,
			ComPtr<ID3D12Device> Device,
			std::shared_ptr<Ideal::ResourceManager> ResourceManager,
			std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> QuadMesh,
			uint32 Width,
			uint32 Height
			);

		void Resize(uint32 Width, uint32 Height, std::shared_ptr<DeferredDeleteManager> DeferredDeleteManager, std::shared_ptr<Ideal::ResourceManager> ResourceManager);

		void PostProcess(
			std::shared_ptr<Ideal::D3D12Texture> OriginTexture,
			std::shared_ptr<Ideal::D3D12Texture> BlurTexture0,
			std::shared_ptr<Ideal::D3D12Texture> BlurTexture1,
			std::shared_ptr<Ideal::D3D12Viewport> Viewport,
			ComPtr<ID3D12Device> Device,
			ComPtr<ID3D12GraphicsCommandList> CommandList,
			std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap,
			std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool
		);

	private:
		void CreateCompositeRootSignature(ComPtr<ID3D12Device> Device);
		void CreateCompositePipelineState(ComPtr<ID3D12Device> Device);

	private:
		ComPtr<ID3D12RootSignature> m_compositeRootSignature;
		ComPtr<ID3D12PipelineState> m_compositePipelineState;

	private:
		std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> m_quadMesh;
		std::shared_ptr<Ideal::D3D12Texture> m_compositeTexture;

		std::shared_ptr<Ideal::D3D12Shader> m_compositeVS;
		std::shared_ptr<Ideal::D3D12Shader> m_compositePS;

	};
}

