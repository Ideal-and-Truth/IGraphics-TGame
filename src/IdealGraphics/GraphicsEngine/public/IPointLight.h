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
		virtual void SetLightColor(const Color& LightColor) abstract;
		virtual void SetPosition(const Vector3& LightPosition) abstract;
		virtual void SetRange(const float& Range) abstract;
		virtual void SetIntensity(const float& Intensity) abstract;
	};
}