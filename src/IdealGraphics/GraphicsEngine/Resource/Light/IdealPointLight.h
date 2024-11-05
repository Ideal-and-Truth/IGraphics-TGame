#pragma once
#include "GraphicsEngine/public/IPointLight.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

namespace Ideal
{
	class IdealPointLight : public IPointLight
	{
	public:
		IdealPointLight();
		virtual ~IdealPointLight();

	public:
		virtual Color GetLightColor() override;
		virtual void SetLightColor(const Color& LightColor) override;

		virtual Vector3 GetPosition() override { return m_pointLight.Position; }
		virtual void SetPosition(const Vector3& LightPosition) override;

		virtual float GetRange() override { return m_pointLight.Range; }
		virtual void SetRange(const float& Range) override;

		virtual float GetIntensity() override { return m_pointLight.Intensity; }
		virtual void SetIntensity(const float& Intensity) override;

		virtual void SetNoShadowCasting(bool Active) override;
		virtual bool GetIsNoShadowCasting() override;

	public:
		PointLight const& GetPointLightDesc() { return m_pointLight; }

	private:
		PointLight m_pointLight;
	};
}