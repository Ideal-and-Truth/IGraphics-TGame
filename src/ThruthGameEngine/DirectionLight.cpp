#include "DirectionLight.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Entity.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::DirectionLight)



Truth::DirectionLight::DirectionLight()
	: Component()
	, m_isRendering(true)
	, m_directionalLight(nullptr)
	, m_direction{ 1.0f, 1.0f, 1.0f }
	, m_intensity(1.0f)
	, m_diffuseColor{ 1.0f, 1.0f, 1.0f, 1.0f }
	, m_ambientColor{ 1.0f, 1.0f, 1.0f, 1.0f }
{
	m_name = "DirectionLight";
}

Truth::DirectionLight::~DirectionLight()
{
}

void Truth::DirectionLight::SetLight()
{
	m_directionalLight = m_managers.lock()->Graphics()->CreateDirectionalLight();
}

void Truth::DirectionLight::SetIntensity()
{
	m_directionalLight->SetIntensity(m_intensity);
}

void Truth::DirectionLight::SetDiffuse()
{
	m_directionalLight->SetDiffuseColor(m_diffuseColor);
}

void Truth::DirectionLight::SetAmbient()
{
	m_directionalLight->SetAmbientColor(m_ambientColor);
}

void Truth::DirectionLight::Initalize()
{
	SetLight();
	SetIntensity();
	SetDiffuse();
	SetAmbient();
}

void Truth::DirectionLight::ApplyTransform()
{
	m_directionalLight->SetDirection(m_direction);
}

#ifdef EDITOR_MODE
void Truth::DirectionLight::EditorSetValue()
{
	SetIntensity();
	SetDiffuse();
	SetAmbient();
}
#endif // _DEBUG

