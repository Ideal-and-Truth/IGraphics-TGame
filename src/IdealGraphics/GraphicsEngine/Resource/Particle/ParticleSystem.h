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

// 아래의 Particle System Enum은 어딘가로 옮겨야 쓸 것 같다
namespace Ideal
{
	namespace ParticleMenu
	{
		// 범위 설정에 따른 Enum
		enum class ERangeMode
		{
			Constant = 0, // 상수 
			Curve, 
			RandomBetweenConstants,	// 두 개의 상수 사이의 랜덤 값
			RandomBetweenCurves,	// 두 개의 커브 사이의 랜덤 값
		};

		enum class ERendererMode
		{
			Billboard,	// 카메라만 바라봄
			Mesh,	// 매쉬 형태로 매쉬에 머테리얼이 입혀진다.
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
		// 랜더 모드를 설정 : 매쉬 형태 Or 빌보드 형태인지
		void SetRendererMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode);
		void SetRendererMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh);
		void SetRendererMaterial(std::shared_ptr<Ideal::ParticleMaterial> ParticleRendererMaterial);

		// 만약 두 개의 데이터가 필요하지 않을 경우 앞의 값만 입력
		void SetCustomData(Ideal::ParticleMenu::ECustomData CustomData, Ideal::ParticleMenu::ECustomDataParameter CustomDataParameter, Ideal::ParticleMenu::ERangeMode RangeMode, float CustomDataFloat, float CustomDataFloat2 = 0.f);

		//------Renderer Menu------//
	private:
		Ideal::ParticleMenu::ERendererMode m_Renderer_Mode;
		// 만약 아래의 Mesh가 ERendererMode가 Mesh가 아닐 경우 사각형 고정이 될 것이다. 사각형에 띄워야 하니까
		std::weak_ptr<Ideal::IdealMesh<ParticleVertexTest>> m_Renderer_Mesh;
		std::weak_ptr<Ideal::ParticleMaterial> m_particleMaterial;

		//----CB Data----//
	private:
		CB_ParticleSystem m_cbParticleSystem;
	};
}