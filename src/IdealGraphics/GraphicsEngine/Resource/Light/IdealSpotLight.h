#pragma once
#include "GraphicsEngine/Resource/Light/IdealLight.h"

namespace Ideal
{
	class IdealSpotLight : public IdealLight
	{
	public:
		IdealSpotLight();
		virtual ~IdealSpotLight();

	public:
		void SetLightColor(const Color& LightColor);
		void SetDirection(const Vector4& Direction);
		void SetPosition(const Vector3& Position);
		void SetSpotAngle(float SpotAngle);
		void SetRange(float Range);
		void SetIntensity(float Intensity);

	private:
		SpotLight m_spotLight;
	};
}