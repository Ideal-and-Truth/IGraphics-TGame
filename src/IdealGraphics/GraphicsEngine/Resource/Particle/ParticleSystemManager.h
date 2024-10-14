#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12GraphicsCommandList;

namespace Ideal
{
	class D3D12DescriptorHeap;
	class D3D12DynamicConstantBufferAllocator;
	class ParticleSystem;
	class D3D12Shader;
}

namespace Ideal
{
	class ParticleSystemManager
	{
	public:
		ParticleSystemManager();
		virtual ~ParticleSystemManager();

	public:
		void Init(ComPtr<ID3D12Device> Device);

		ComPtr<ID3D12RootSignature> GetRootSignature();
		std::shared_ptr<Ideal::D3D12Shader> GetMeshVS();
		std::shared_ptr<Ideal::D3D12Shader> GetBillboardVS();

		void SetMeshVS(std::shared_ptr<Ideal::D3D12Shader> Shader);
		void SetBillboardVS(std::shared_ptr<Ideal::D3D12Shader> Shader);

	private:
		void CreateRootSignature(ComPtr<ID3D12Device> Device);

	public:
		void AddParticleSystemNoTransparency(std::shared_ptr<Ideal::ParticleSystem> ParticleSystem);
		void AddParticleSystem(std::shared_ptr<Ideal::ParticleSystem> ParticleSystem);
		void DeleteParticleSystem(std::shared_ptr<Ideal::ParticleSystem>& ParticleSystem);

		void DrawParticles(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, CB_Global* CB_GlobalData);

	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
		std::shared_ptr<Ideal::D3D12Shader> m_RENDER_MODE_MESH_VS;
		std::shared_ptr<Ideal::D3D12Shader> m_RENDER_MODE_BILLBOARD_VS;

	private:
		std::vector<std::shared_ptr<Ideal::ParticleSystem>> m_particlesNoTransparency;
		std::vector<std::shared_ptr<Ideal::ParticleSystem>> m_particles;
	};
}
