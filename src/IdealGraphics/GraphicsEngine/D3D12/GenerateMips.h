#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;
struct ID3D12GraphicsCommandList;


namespace Ideal
{
	class D3D12Shader;
	class D3D12Texture;
	class D3D12DynamicConstantBufferAllocator;
	class D3D12DescriptorHeap;
}

namespace Ideal
{
	class GenerateMips
	{
	public:

	public:
		void Init(ComPtr<ID3D12Device> Device, std::shared_ptr<Ideal::D3D12Shader> Shader);

		// Descriptor handle을 할당 받아야 한다.
		void Generate(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, std::shared_ptr<Ideal::D3D12Texture> Texture, CB_GenerateMipsInfo* MipsInfoData);

	private:
		void CreateRootSignature(ComPtr<ID3D12Device> Device);
		void CreatePipelineState(ComPtr<ID3D12Device> Device);
		 
	private:


	private:
		std::shared_ptr<Ideal::D3D12Shader> m_GenerateMipsCS;
		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12PipelineState> m_pipelineState;
	};
}
