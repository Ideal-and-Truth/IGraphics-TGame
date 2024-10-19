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
			std::shared_ptr<Ideal::D3D12Shader> DownSampleShader,
			std::shared_ptr<Ideal::D3D12Shader> BlurShader,
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

		std::shared_ptr<Ideal::D3D12Texture> GetBlurTexture();

	private:
		void CreateDownSampleRootSignature(ComPtr<ID3D12Device> Device);
		void CreateDownSamplePipelineState(ComPtr<ID3D12Device> Device);

		void CreateBlurRootSignature(ComPtr<ID3D12Device> Device);
		void CreateBlurPipelineState(ComPtr<ID3D12Device> Device);

	private:
		void PostProcess_DownSample(
			std::shared_ptr<Ideal::D3D12Texture> BloomTexture,
			std::shared_ptr<Ideal::D3D12Texture> OutTexture,
			float Threshold,
			ComPtr<ID3D12Device> Device,
			ComPtr<ID3D12GraphicsCommandList> CommandList,
			std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap,
			std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool,
			uint32 Width,
			uint32 Height
		);

		void PostProcess_Blur(
			std::shared_ptr<Ideal::D3D12Texture> DownSampleTexture,
			std::shared_ptr<Ideal::D3D12Texture> OutBlurTexture
		,
			uint32 direction
		,
			ComPtr<ID3D12Device> Device,
			ComPtr<ID3D12GraphicsCommandList> CommandList,
			std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap,
			std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool,
			uint32 Width,
			uint32 Height
		);

	private:
		uint32 m_width;
		uint32 m_height;

		std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> m_quadMesh;

		std::shared_ptr<Ideal::D3D12Shader> m_blurCS;
		std::shared_ptr<Ideal::D3D12Shader> m_downSampleCS;

		ComPtr<ID3D12RootSignature> m_downSampleRootSignature;
		ComPtr<ID3D12PipelineState> m_downSamplePipelineState;

		ComPtr<ID3D12RootSignature> m_blurRootSignature;
		ComPtr<ID3D12PipelineState> m_blurPipelineState;

	private:
		// DownSample
		float m_threshold = 0.5f;
		std::shared_ptr<Ideal::D3D12Texture> m_downSampleTexture0;

	private:
		// Blur
		void InitBlurCB();

		std::shared_ptr<Ideal::D3D12Texture> m_blurTexture[2];
		CB_Blur m_cbBlur;
	};
}