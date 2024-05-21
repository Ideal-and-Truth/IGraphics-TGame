#pragma once
#include "GraphicsEngine/public/ISpotLight.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

namespace Ideal
{
	class IdealSpotLight : public ISpotLight
	{
	public:
		IdealSpotLight();
		virtual ~IdealSpotLight();

	public:
		virtual void SetLightColor(const Color& LightColor) override;
		virtual void SetDirection(const Vector4& Direction) override;
		virtual void SetPosition(const Vector3& Position) override;
		virtual void SetSpotAngle(const float& SpotAngle) override;
		virtual void SetRange(const float& Range) override;
		virtual void SetIntensity(const float& Intensity) override;

	public:
		SpotLight const& GetSpotLightDesc() { return m_spotLight; }

	private:
		SpotLight m_spotLight;
	};
}