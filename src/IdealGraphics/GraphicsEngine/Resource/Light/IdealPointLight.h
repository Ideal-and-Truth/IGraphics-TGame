#pragma once
#include "GraphicsEngine/Resource/Light/IdealLight.h"

namespace Ideal
{
	class IdealPointLight : public IdealLight
	{
	public:
		IdealPointLight();
		virtual ~IdealPointLight();

	public:
		void SetLightColor(const Color& LightColor);
		void SetPosition(const Vector3& LightPosition);
		void SetRange(const float& Range);
		void SetIntensity(const float& Intensity);

	private:
		PointLight m_pointLight;
	};
}