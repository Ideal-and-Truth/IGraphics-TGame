#pragma once
#include <memory>
#include "../Utils/SimpleMath.h"
#include "IGraph.h"

namespace Ideal
{
	class IMesh;
	class IParticleMaterial;
}
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
			DirectX::SimpleMath::Vector4 CustomData1;
			DirectX::SimpleMath::Vector4 CustomData2;
			float Time;
		};

		enum class ECustomData
		{
			CustomData_1 = 0,
			CustomData_2,
		};
		enum class ECustomDataParameter : unsigned int
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
	class IParticleSystem
	{
	public:
		IParticleSystem() {};
		virtual ~IParticleSystem() {};

	public:
		virtual void SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial) abstract;
		virtual std::weak_ptr<Ideal::IParticleMaterial> GetMaterial() abstract;

		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& Transform) abstract;
		virtual const DirectX::SimpleMath::Matrix& GetTransformMatrix() const abstract;

		virtual void SetActive(bool IsActive) abstract;
		virtual bool GetActive() abstract;

		virtual void SetDeltaTime(float DT) abstract;

		virtual void Play() abstract;
		virtual void Pause() abstract;

		virtual void SetStopWhenFinished(bool StopWhenFinished) abstract;
		virtual void SetPlayOnWake(bool PlayOnWake) abstract;

	public:
		virtual void SetStartColor(const DirectX::SimpleMath::Color& StartColor) abstract;
		virtual DirectX::SimpleMath::Color& GetStartColor() abstract;

		virtual void SetStartSize(float Size) abstract;

		// 생명 시간
		virtual void SetStartLifetime(float Time) abstract;
		virtual float GetStartLifetime() abstract;

		// 재생 속도 값
		virtual void SetSimulationSpeed(float Speed) abstract;
		virtual float GetSimulationSpeed() abstract;

		// 총 지속 시간
		virtual void SetDuration(float Time) abstract;
		virtual float GetDuration() abstract;

		virtual void SetLoop(bool Loop) abstract;
		virtual bool GetLoop() abstract;

		//------Color Over Lifetime------//
		// 생명 시간 동안 색상 값이 바뀔지를 결정. 그라디언트 그래프를 통해 조작함
		virtual void SetColorOverLifetime(bool Active) abstract;
		virtual Ideal::IGradient& GetColorOverLifetimeGradientGraph() abstract;

		//----Rotation Over Lifetime----//
		// 생명 시간 동안 회전 값 조절. 베지어 커브 그래프 사용
		virtual void SetRotationOverLifetime(bool Active) abstract;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisX() abstract;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisY() abstract;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisZ() abstract;

		//----Size Over Lifetime----//
		virtual void SetSizeOverLifetime(bool Active) abstract;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisX() abstract;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisY() abstract;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisZ() abstract;

		//----------Custom Data---------//
		virtual Ideal::IBezierCurve& GetCustomData1X() abstract;
		virtual Ideal::IBezierCurve& GetCustomData1Y() abstract;
		virtual Ideal::IBezierCurve& GetCustomData1Z() abstract;
		virtual Ideal::IBezierCurve& GetCustomData1W() abstract;

		virtual Ideal::IBezierCurve& GetCustomData2X() abstract;
		virtual Ideal::IBezierCurve& GetCustomData2Y() abstract;
		virtual Ideal::IBezierCurve& GetCustomData2Z() abstract;
		virtual Ideal::IBezierCurve& GetCustomData2W() abstract;

		//-----------Renderer-----------//
		virtual void SetRenderMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode) abstract;
		virtual void SetRenderMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh) abstract;
		virtual void SetRenderMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleRendererMaterial) abstract;
	};
}