#include "SpotLight.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Entity.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::SpotLight)

Truth::SpotLight::SpotLight()
	: Component()
	, m_isRendering(true)
	, m_spotLight(nullptr)
{
	m_name = "SpotLight";
}

Truth::SpotLight::~SpotLight()
{
}

void Truth::SpotLight::SetLight()
{
	m_spotLight = m_managers.lock()->Graphics()->CreateSpotLight();
}

void Truth::SpotLight::SetIntensity()
{
	m_spotLight->SetIntensity(m_intensity);
}

void Truth::SpotLight::SetColor()
{
	m_spotLight->SetLightColor(m_lightColor);
}

void Truth::SpotLight::Initalize()
{
	SetLight();
	SetIntensity();
	SetColor();
}

void Truth::SpotLight::ApplyTransform()
{
	m_spotLight->SetPosition(m_position);
	// m_spotLight->SetDirection(m_direction);
}

#ifdef EDITOR_MODE
void Truth::SpotLight::EditorSetValue()
{
	SetIntensity();
	SetColor();
}
#endif // _DEBUG

