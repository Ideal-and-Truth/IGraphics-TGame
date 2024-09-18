#pragma once
#include <memory>
#include "../Utils/SimpleMath.h"

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

		// 복사 생성자를 만들어야 할지도
		// ON, OFF 기능도
	public:
		virtual void SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial) abstract;
		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& Transform) abstract;
		virtual const DirectX::SimpleMath::Matrix& GetTransformMatrix() const abstract;

	public:
		virtual void SetStartColor(const DirectX::SimpleMath::Color& StartColor) abstract;

		//---------Renderer---------//
		virtual void SetRenderMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode) abstract;
		virtual void SetRenderMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh) abstract;
		virtual void SetRenderMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleRendererMaterial) abstract;
	};
}