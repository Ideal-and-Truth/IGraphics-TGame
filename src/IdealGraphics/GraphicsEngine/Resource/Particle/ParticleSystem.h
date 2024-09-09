#pragma once
#include "IParticleSystem.h"
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;

namespace Ideal
{
	class ParticleMaterial;
	class IMesh;
	template <typename> class IdealMesh;
	class D3D12Shader;
}

// �Ʒ��� Particle System Enum�� ��򰡷� �Űܾ� �� �� ����
namespace Ideal
{
	namespace ParticleMenu
	{
		// ���� ������ ���� Enum
		enum class ERangeMode
		{
			Constant = 0, // ��� 
			Curve, 
			RandomBetweenConstants,	// �� ���� ��� ������ ���� ��
			RandomBetweenCurves,	// �� ���� Ŀ�� ������ ���� ��
		};

		enum class ERendererMode
		{
			Billboard,	// ī�޶� �ٶ�
			Mesh,	// �Ž� ���·� �Ž��� ���׸����� ��������.
		};

		struct ShaderData
		{
			float Time;

			Vector4 CustomData1;
			Vector4 CustomData2;
		};

		enum class ECustomData
		{
			CustomData_1 = 0,
			CustomData_2,
		};
		enum class ECustomDataParameter : uint32
		{
			CustomData_x = 0,
			CustomData_y,
			CustomData_z,
			CustomData_w
		};
	}
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

	private:
		void CreatePipelineState(ComPtr<ID3D12Device> Device);

	private:
		ComPtr<ID3D12PipelineState> m_pipelineState;
		ComPtr<ID3D12RootSignature> m_rootSignature;
		std::shared_ptr<Ideal::D3D12Shader> m_vs;
		std::shared_ptr<Ideal::D3D12Shader> m_ps;

	public:
		//--------Renderer---------//
		// ���� ��带 ���� : �Ž� ���� Or ������ ��������
		void SetRendererMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode);
		void SetRendererMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh);
		void SetRendererMaterial(std::shared_ptr<Ideal::ParticleMaterial> ParticleRendererMaterial);

		// ���� �� ���� �����Ͱ� �ʿ����� ���� ��� ���� ���� �Է�
		void SetCustomData(Ideal::ParticleMenu::ECustomData CustomData, Ideal::ParticleMenu::ECustomDataParameter CustomDataParameter, Ideal::ParticleMenu::ERangeMode RangeMode, float CustomDataFloat, float CustomDataFloat2 = 0.f);

		//------Renderer Menu------//
	private:
		Ideal::ParticleMenu::ERendererMode m_Renderer_Mode;
		// ���� �Ʒ��� Mesh�� ERendererMode�� Mesh�� �ƴ� ��� �簢�� ������ �� ���̴�. �簢���� ����� �ϴϱ�
		std::weak_ptr<Ideal::IdealMesh<ParticleVertexTest>> m_Renderer_Mesh;
		std::weak_ptr<Ideal::ParticleMaterial> m_particleMaterial;

		//----CB Data----//
	private:
		CB_ParticleSystem m_cbParticleSystem;
	};
}