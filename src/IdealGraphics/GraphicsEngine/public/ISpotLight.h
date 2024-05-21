#pragma once
#include "../Utils/SimpleMath.h"
#include "ILight.h"
using namespace DirectX::SimpleMath;

namespace Ideal
{
	class ISpotLight : public ILight
	{
	public:
		ISpotLight()
			: ILight(ELightType::Spot)
		{}
		virtual ~ISpotLight() {}

	public:
		virtual void SetLightColor(const Color& LightColor) abstract;
		virtual void SetDirection(const Vector4& Direction) abstract;
		virtual void SetPosition(const Vector3& Position) abstract;
		virtual void SetSpotAngle(const float& SpotAngle) abstract;
		virtual void SetRange(const float& Range) abstract;
		virtual void SetIntensity(const float& Intensity) abstract;
	};
}