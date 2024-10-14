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
		virtual std::weak_ptr<Ideal::IParticleMaterial> GetMaterial() override;

		virtual void SetTransformMatrix(const Matrix& Transform) override;
		virtual const Matrix& GetTransformMatrix() const override;

		virtual void SetActive(bool IsActive) override;
		virtual bool GetActive() override;

		virtual void SetDeltaTime(float DT) override;
		
		virtual void Play() override;
		virtual void Resume() override;	// 일시 정지 했을 경우 지금 시간부터 시작
		virtual void Pause() override;
		virtual void SetStopWhenFinished(bool StopWhenFinished) override;
		virtual void SetPlayOnWake(bool PlayOnWake) override;
		virtual float GetCurrentDurationTime() override;
	public:
		void Init(ComPtr<ID3D12Device> Device, ComPtr<ID3D12RootSignature> RootSignature, std::shared_ptr<Ideal::ParticleMaterial> ParticleMaterial);
		void DrawParticle(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);

		void SetMeshVS(std::shared_ptr<Ideal::D3D12Shader> Shader);
		void SetBillboardVS(std::shared_ptr<Ideal::D3D12Shader> Shader);
		void SetBillboardGS(std::shared_ptr<Ideal::D3D12Shader> Shader);

	private:
		void RENDER_MODE_MESH_CreatePipelineState(ComPtr<ID3D12Device> Device);
		void RENDER_MODE_BILLBOARD_CreatePipelineState(ComPtr<ID3D12Device> Device);

	private:
		ComPtr<ID3D12PipelineState> m_RENDER_MODE_MESH_pipelineState;
		ComPtr<ID3D12PipelineState> m_RENDER_MODE_BILLBOARD_pipelineState;
		ComPtr<ID3D12RootSignature> m_rootSignature;
		//ComPtr<ID3D12RootSignature> m_RENDER_MODE_BILLBOARD_RootSignature;
		std::shared_ptr<Ideal::D3D12Shader> m_RENDER_MODE_MESH_VS;
		std::shared_ptr<Ideal::D3D12Shader> m_RENDER_MODE_BILLBOARD_VS;
		std::shared_ptr<Ideal::D3D12Shader> m_RENDER_MODE_BILLBOARD_GS;
		std::shared_ptr<Ideal::D3D12Shader> m_ps;
		std::shared_ptr<Ideal::D3D12PipelineStateObject> m_pso;

		//----------------------------Interface---------------------------//
	public:
		virtual void SetStartColor(const DirectX::SimpleMath::Color& StartColor) override;
		virtual DirectX::SimpleMath::Color& GetStartColor() override;

		virtual void SetStartSize(float Size) override;

		virtual void SetStartLifetime(float Time) override;
		virtual float GetStartLifetime() override;

		virtual void SetSimulationSpeed(float Speed) override;
		virtual float GetSimulationSpeed() override;

		virtual void SetDuration(float Time) override;
		virtual float GetDuration() override;

		virtual void SetLoop(bool Loop) override;
		virtual bool GetLoop() override;

		//------Color Over Lifetime------//
		virtual void SetColorOverLifetime(bool Active) override;
		virtual Ideal::IGradient& GetColorOverLifetimeGradientGraph() override;
		void UpdateColorOverLifetime();

		//----Size Over Lifetime----//
		virtual void SetSizeOverLifetime(bool Active) override;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisX() override;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisY() override;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisZ() override;
		void UpdateSizeOverLifetime();

		//----Rotation Over Lifetime----//
		virtual void SetRotationOverLifetime(bool Active) override;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisX() override;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisY() override;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisZ() override;

		//----------Custom Data---------//
		virtual Ideal::IBezierCurve& GetCustomData1X() override;
		virtual Ideal::IBezierCurve& GetCustomData1Y() override;
		virtual Ideal::IBezierCurve& GetCustomData1Z() override;
		virtual Ideal::IBezierCurve& GetCustomData1W() override;

		virtual Ideal::IBezierCurve& GetCustomData2X() override;
		virtual Ideal::IBezierCurve& GetCustomData2Y() override;
		virtual Ideal::IBezierCurve& GetCustomData2Z() override;
		virtual Ideal::IBezierCurve& GetCustomData2W() override;

		//--------Renderer---------//
		// 랜더 모드를 설정 : 매쉬 형태 Or 빌보드 형태인지
		virtual void SetRenderMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode) override;
		virtual void SetRenderMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh) override;
		virtual void SetRenderMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleRendererMaterial) override;

		// 만약 두 개의 데이터가 필요하지 않을 경우 앞의 값만 입력
		void SetCustomData(Ideal::ParticleMenu::ECustomData CustomData, Ideal::ParticleMenu::ECustomDataParameter CustomDataParameter, Ideal::ParticleMenu::ERangeMode RangeMode, float CustomDataFloat, float CustomDataFloat2 = 0.f);

	private:
		void DrawRenderMesh(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);
		void DrawRenderBillboard(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);

		void UpdateCustomData();
		void UpdateLifeTime();
	private:
		float m_deltaTime = 0.f;
		bool m_isPlaying = true;
		bool m_stopWhenFinished = false;
		bool m_playOnWake = true;
		bool m_isActive = true;
		bool m_isLoop = true;
		float m_duration = 1.f;
		float m_currentDurationTime = 0.f;
		float m_currentTime = 0.f;
		Color m_startColor = Color(1, 1, 1, 1);
		float m_startSize = 1.f;
		float m_startLifetime = 1.f;	//1 은 임시
		float m_simulationSpeed = 1.f;

		//------Color Over Lifetime------//
		bool m_isUseColorOverLifetime = false;
		Ideal::Gradient m_ColorOverLifetimeGradientGraph;

		//----Size Over Lifetime----//
		bool m_isSizeOverLifetime = false;
		Ideal::BezierCurve m_SizeOverLifetimeAxisX;
		Ideal::BezierCurve m_SizeOverLifetimeAxisY;
		Ideal::BezierCurve m_SizeOverLifetimeAxisZ;

		//----Rotation Over Lifetime----//
		bool m_isRotationOverLifetime = false;
		Ideal::BezierCurve m_RotationOverLifetimeAxisX;
		Ideal::BezierCurve m_RotationOverLifetimeAxisY;
		Ideal::BezierCurve m_RotationOverLifetimeAxisZ;

		//----------Custom Data---------//
		Ideal::BezierCurve m_CustomData1_X;
		Ideal::BezierCurve m_CustomData1_Y;
		Ideal::BezierCurve m_CustomData1_Z;
		Ideal::BezierCurve m_CustomData1_W;

		Ideal::BezierCurve m_CustomData2_X;
		Ideal::BezierCurve m_CustomData2_Y;
		Ideal::BezierCurve m_CustomData2_Z;
		Ideal::BezierCurve m_CustomData2_W;

		//------Renderer Menu------//
		Ideal::ParticleMenu::ERendererMode m_Renderer_Mode;
		// 만약 아래의 Mesh가 ERendererMode가 Mesh가 아닐 경우 사각형 고정이 될 것이다. 사각형에 띄워야 하니까
		std::weak_ptr<Ideal::IdealMesh<ParticleMeshVertex>> m_Renderer_Mesh;
		std::weak_ptr<Ideal::ParticleMaterial> m_particleMaterial;

		//----CB Data----//
	private:
		CB_ParticleSystem m_cbParticleSystem;
		CB_Transform m_cbTransform;
		Matrix m_transform;
	};
}