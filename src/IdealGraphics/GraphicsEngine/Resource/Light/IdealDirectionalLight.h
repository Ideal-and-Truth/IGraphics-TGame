#pragma once
#include "GraphicsEngine/public/IDirectionalLight.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

namespace Ideal
{
	class IdealDirectionalLight : public IDirectionalLight
	{
	public:
		IdealDirectionalLight();
		virtual ~IdealDirectionalLight();

	public:
		virtual void SetAmbientColor(const Color& LightColor) override;
		virtual void SetDiffuseColor(const Color& LightColor) override;
		virtual void SetDirection(const Vector3& Direction) override;
		virtual void SetIntensity(const float& Intensity) override;

	public:
		DirectionalLight const& GetDirectionalLightDesc() { return m_directionalLight; }

	private:
		DirectionalLight m_directionalLight;
	};
}