#include "PointLight.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Entity.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::PointLight)

Truth::PointLight::PointLight()
	: Component()
	, m_isRendering(true)
	, m_pointLight(nullptr)
	, m_position{ 0.0f, 0.0f, 0.0f }
	, m_radius(1.0f)
	, m_lightColor{ 1.0f, 1.0f, 1.0f, 1.0f }
	, m_intensity(1.0f)
{
	m_name = "PointLight";
}

Truth::PointLight::~PointLight()
{
}

void Truth::PointLight::SetLight()
{
	m_pointLight = m_managers.lock()->Graphics()->CreatePointLight();
}

void Truth::PointLight::SetIntensity()
{
	m_pointLight->SetIntensity(m_intensity);
}

void Truth::PointLight::SetColor()
{
	m_pointLight->SetLightColor(m_lightColor);
}

void Truth::PointLight::SetRange()
{
	m_pointLight->SetRange(m_radius);
}

void Truth::PointLight::Initialize()
{
	SetLight();
	SetIntensity();
	SetColor();
	SetRange();
}

void Truth::PointLight::ApplyTransform()
{
	m_pointLight->SetPosition(m_position);
	m_pointLight->SetRange(m_radius);
}

#ifdef EDITOR_MODE
void Truth::PointLight::EditorSetValue()
{
	SetIntensity();
	SetColor();
	SetRange();
}
#endif // _DEBUG

