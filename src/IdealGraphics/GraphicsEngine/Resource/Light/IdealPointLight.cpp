                     #include "IdealPointLight.h"

Ideal::IdealPointLight::IdealPointLight()
{
	m_pointLight.Color = Color(1.f, 1.f, 1.f, 1.f);
	m_pointLight.Position = Vector3(0.f, 0.f, 0.f);
	m_pointLight.Range = 10.f;
	m_pointLight.Intensity = 1.f;
}

Ideal::IdealPointLight::~IdealPointLight()
{

}

DirectX::SimpleMath::Color Ideal::IdealPointLight::GetLightColor()
{
	return m_pointLight.Color;
}

void Ideal::IdealPointLight::SetLightColor(const Color& LightColor)
{
	m_pointLight.Color = LightColor;
}

void Ideal::IdealPointLight::SetPosition(const Vector3& LightPosition)
{
	m_pointLight.Position = LightPosition;
}

void Ideal::IdealPointLight::SetRange(const float& Range)
{
	m_pointLight.Range = Range;
}

void Ideal::IdealPointLight::SetIntensity(const float& Intensity)
{
	m_pointLight.Intensity = Intensity;
}
