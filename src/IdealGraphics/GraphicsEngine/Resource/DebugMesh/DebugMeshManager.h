#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12GraphicsCommandList;
struct ID3D12PipelineState;

namespace Ideal
{
	class D3D12DescriptorHeap;
	class D3D12DynamicConstantBufferAllocator;
	class IdealStaticMeshObject;
	class D3D12Shader;
}

namespace Ideal
{
	class DebugMeshManager
	{
	public:
		void Init(ComPtr<ID3D12Device> Device);

	private:
		void CreateRootSignature(ComPtr<ID3D12Device> Device);
		void CreatePipelineState(ComPtr<ID3D12Device> Device);

	public:
		void AddDebugMesh(std::shared_ptr<Ideal::IdealStaticMeshObject> DebugMesh);
		void DeleteDebugMesh(std::shared_ptr<Ideal::IdealStaticMeshObject> DebugMesh);

		void SetVS(std::shared_ptr<Ideal::D3D12Shader> Shader);
		void SetPS(std::shared_ptr<Ideal::D3D12Shader> Shader);

		void DrawDebugMeshes(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, CB_Global* CB_GlobalData);

	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12PipelineState> m_pipelineState;
		std::shared_ptr<Ideal::D3D12Shader> m_vs;
		std::shared_ptr<Ideal::D3D12Shader> m_ps;

	private:
		std::vector<std::weak_ptr<Ideal::IdealStaticMeshObject>> m_meshes;

	};
}
