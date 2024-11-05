#pragma once
#include "../Utils/SimpleMath.h"
#include "ILight.h"
using namespace DirectX::SimpleMath;

namespace Ideal
{
	class IPointLight : public ILight
	{
	public:
		IPointLight()
			: ILight(ELightType::Point)
		{}
		virtual ~IPointLight() {}

	public:
		virtual Color GetLightColor() abstract;
		virtual void SetLightColor(const Color& LightColor) abstract;

		virtual Vector3 GetPosition() abstract;
		virtual void SetPosition(const Vector3& LightPosition) abstract;

		virtual float GetRange() abstract;
		virtual void SetRange(const float& Range) abstract;

		virtual float GetIntensity() abstract;
		virtual void SetIntensity(const float& Intensity) abstract;

		virtual void SetNoShadowCasting(bool Active) abstract;
		virtual bool GetIsNoShadowCasting() abstract;
	};
}