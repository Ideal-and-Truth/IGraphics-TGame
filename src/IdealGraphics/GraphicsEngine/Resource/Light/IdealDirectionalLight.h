#pragma once
#include "GraphicsEngine/Resource/Light/IdealLight.h"

namespace Ideal
{
	class IdealDirectionalLight : public IdealLight
	{
	public:
		IdealDirectionalLight();
		virtual ~IdealDirectionalLight();

	public:
		void SetAmbientColor(const Color& LightColor);
		void SetDiffuseColor(const Color& LightColor);
		void SetDirection(const Vector3& Direction);
		void SetIntensity(const float& Intensity);

	private:
		DirectionalLight m_directionalLight;
	};
}