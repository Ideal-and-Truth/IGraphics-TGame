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
	class BloomPass
	{
	public:
		void InitBloomPass(
			ComPtr<ID3D12Device> Device,
			std::shared_ptr<Ideal::ResourceManager> ResourceManager,
			std::shared_ptr<Ideal::D3D12Shader> BlurShaderVS,
			std::shared_ptr<Ideal::D3D12Shader> BlurShaderPS,
			std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> QuadMesh,
			uint32 Width, uint32 Height
		);
		void PostProcess(
			std::shared_ptr<Ideal::D3D12Texture> BeforeTexture,
			std::shared_ptr<Ideal::D3D12Texture> BloomTexture,
			std::shared_ptr<Ideal::D3D12Texture> RenderTarget,
			std::shared_ptr<Ideal::D3D12Viewport> Viewport,
			ComPtr<ID3D12Device> Device,
			ComPtr<ID3D12GraphicsCommandList> CommandList,
			std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap,
			std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool
		);

		void Resize(uint32 Width, uint32 Height, std::shared_ptr<DeferredDeleteManager> DeferredDeleteManager, std::shared_ptr<Ideal::ResourceManager> ResourceManager);

	private:
		void CreateBlurRootSignature(ComPtr<ID3D12Device> Device);
		void CreateBlurPipelineState(ComPtr<ID3D12Device> Device);

	private:
		void PostProcess_BLUR(
			std::shared_ptr<Ideal::D3D12Texture> BloomTexture,
			std::shared_ptr<Ideal::D3D12Texture> OutTexture,
			ComPtr<ID3D12Device> Device,
			ComPtr<ID3D12GraphicsCommandList> CommandList,
			std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap,
			std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool
		);

	private:
		std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> m_quadMesh;

		std::shared_ptr<Ideal::D3D12Shader> m_blurShaderVS;
		std::shared_ptr<Ideal::D3D12Shader> m_blurShaderPS;
		ComPtr<ID3D12RootSignature> m_blurRootSignature;
		ComPtr<ID3D12PipelineState> m_blurPipelineState;

	private:
		// Blur Texture
		std::shared_ptr<Ideal::D3D12Viewport> m_viewportBlur0;
		std::shared_ptr<Ideal::D3D12Texture> m_blurTexture0;
	};
}