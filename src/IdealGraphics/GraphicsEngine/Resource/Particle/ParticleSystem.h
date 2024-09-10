#pragma once
#include "IParticleSystem.h"
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;
struct ID3D12GraphicsCommandList;

namespace Ideal
{
	class ParticleMaterial;
	class IMesh;
	template <typename> class IdealMesh;

	class D3D12Shader;
	class D3D12PipelineStateObject;
	class D3D12DescriptorHeap;
	class D3D12DynamicConstantBufferAllocator;
}

namespace Ideal
{
	class ParticleSystem : public IParticleSystem
	{
		// TODO
	public:
		ParticleSystem();
		virtual ~ParticleSystem();

	public:
		virtual void SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial) override;

	public:
		void Init(ComPtr<ID3D12Device> Device, ComPtr<ID3D12RootSignature> RootSignature, std::shared_ptr<Ideal::D3D12Shader> Shader, std::shared_ptr<Ideal::ParticleMaterial> ParticleMaterial);
		void DrawParticle(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);

	private:
		void CreatePipelineState(ComPtr<ID3D12Device> Device);

	private:
		ComPtr<ID3D12PipelineState> m_pipelineState;
		ComPtr<ID3D12RootSignature> m_rootSignature;
		std::shared_ptr<Ideal::D3D12Shader> m_vs;
		std::shared_ptr<Ideal::D3D12Shader> m_ps;
		std::shared_ptr<Ideal::D3D12PipelineStateObject> m_pso;

		//----------------------------Interface---------------------------//
	public:
		//--------Renderer---------//
		// ���� ��带 ���� : �Ž� ���� Or ������ ��������
		void SetRenderMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode) override;
		void SetRenderMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh) override;
		void SetRenderMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleRendererMaterial) override;

		// ���� �� ���� �����Ͱ� �ʿ����� ���� ��� ���� ���� �Է�
		void SetCustomData(Ideal::ParticleMenu::ECustomData CustomData, Ideal::ParticleMenu::ECustomDataParameter CustomDataParameter, Ideal::ParticleMenu::ERangeMode RangeMode, float CustomDataFloat, float CustomDataFloat2 = 0.f);

	private:
		void DrawRenderMesh(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);

		//------Renderer Menu------//
	private:
		Ideal::ParticleMenu::ERendererMode m_Renderer_Mode;
		// ���� �Ʒ��� Mesh�� ERendererMode�� Mesh�� �ƴ� ��� �簢�� ������ �� ���̴�. �簢���� ����� �ϴϱ�
		std::weak_ptr<Ideal::IdealMesh<ParticleVertexTest>> m_Renderer_Mesh;
		std::weak_ptr<Ideal::ParticleMaterial> m_particleMaterial;

		//----CB Data----//
	private:
		CB_ParticleSystem m_cbParticleSystem;
		CB_Transform m_cbTransform;
	};
}