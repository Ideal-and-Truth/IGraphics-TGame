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
		virtual void SetTransformMatrix(const Matrix& Transform) override;
		virtual const Matrix& GetTransformMatrix() const override;

		virtual void SetActive(bool IsActive) override;
		virtual bool GetActive() override;

		virtual void SetDeltaTime(float DT) override;
		
		virtual void Play() override;
		virtual void SetStopWhenFinished(bool StopWhenFinished) override;

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
		virtual void SetStartColor(const DirectX::SimpleMath::Color& StartColor) override;
		virtual DirectX::SimpleMath::Color& GetStartColor() override;

		virtual void SetStartLifetime(float Time) override;

		virtual void SetDuration(float Time) override;
		virtual float GetDuration() override;

		virtual void SetLoop(bool Loop) override;
		virtual bool GetLoop() override;

		//------Color Over Lifetime------//
		virtual void SetColorOverLifetime(bool Active) override;
		virtual Ideal::IGradient& GetColorOverLifetimeGradientGraph() override;
		void UpdateColorOverLifetime();

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
		// ���� ��带 ���� : �Ž� ���� Or ������ ��������
		virtual void SetRenderMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode) override;
		virtual void SetRenderMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh) override;
		virtual void SetRenderMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleRendererMaterial) override;

		// ���� �� ���� �����Ͱ� �ʿ����� ���� ��� ���� ���� �Է�
		void SetCustomData(Ideal::ParticleMenu::ECustomData CustomData, Ideal::ParticleMenu::ECustomDataParameter CustomDataParameter, Ideal::ParticleMenu::ERangeMode RangeMode, float CustomDataFloat, float CustomDataFloat2 = 0.f);

	private:
		void DrawRenderMesh(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);

		void UpdateCustomData();
		void UpdateLifeTime();
	private:
		float m_deltaTime = 0.f;
		bool m_stopWhenFinished = false;
		bool m_isActive = true;
		bool m_isLoop = true;
		float m_duration = 1.f;
		float m_currentTime = 0.f;
		Color m_startColor = Color(1, 1, 1, 1);
		float m_startLifetime = 1.f;	//1 �� �ӽ�
			
		//------Color Over Lifetime------//
		bool m_isUseColorOverLifetime = false;
		Ideal::Gradient m_ColorOverLifetimeGradientGraph;

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
		// ���� �Ʒ��� Mesh�� ERendererMode�� Mesh�� �ƴ� ��� �簢�� ������ �� ���̴�. �簢���� ����� �ϴϱ�
		std::weak_ptr<Ideal::IdealMesh<ParticleVertexTest>> m_Renderer_Mesh;
		std::weak_ptr<Ideal::ParticleMaterial> m_particleMaterial;

		//----CB Data----//
	private:
		CB_ParticleSystem m_cbParticleSystem;
		CB_Transform m_cbTransform;
		Matrix m_transform;
	};
}