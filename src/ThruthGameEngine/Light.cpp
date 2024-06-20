#include "Light.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Entity.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Light)



Truth::Light::Light()
	: Component()
	, m_isRendering(true)
	, m_directionalLight(nullptr)
	, m_spotLight(nullptr)
	, m_pointLight(nullptr)
	, m_lightType(LightType::Directional)
	, m_direction(Vector3(1.f, 0.f, 1.f))
	, m_position(Vector3(0.f, 0.f, 0.f))
{
	m_name = "Light";
}

Truth::Light::~Light()
{
}

void Truth::Light::SetLight(LightType InLightType)
{
	if (InLightType == LightType::Directional)
	{
		m_directionalLight = m_managers.lock()->Graphics()->CreateDirectionalLight();
	}
	else if (InLightType == LightType::Spot)
	{
		m_spotLight = m_managers.lock()->Graphics()->CreateSpotLight();
	}
	else if (InLightType == LightType::Point)
	{
		m_pointLight = m_managers.lock()->Graphics()->CreatePointLight();
	}
}

void Truth::Light::AddLight(std::shared_ptr<Ideal::ILight> _light)
{
	m_managers.lock()->Graphics()->AddLight(_light);
}

void Truth::Light::Initalize()
{
	SetLight(m_lightType);
	if (m_lightType == LightType::Directional)
	{
		AddLight(m_directionalLight);
	}
	else if (m_lightType == LightType::Spot)
	{
		AddLight(m_spotLight);
	}
	else if (m_lightType == LightType::Point)
	{
		AddLight(m_pointLight);
	}
}

void Truth::Light::ApplyTransform()
{
	if (m_lightType == LightType::Directional)
	{
		m_directionalLight->SetDirection(m_direction);
	}
	else if (m_lightType == LightType::Spot)
	{
		m_spotLight->SetPosition(m_position);
	}
	else if (m_lightType == LightType::Point)
	{
		m_pointLight->SetPosition(m_position);
	}
}

#ifdef _DEBUG
void Truth::Light::EditorSetValue()
{
	SetLight(m_lightType);
	if (m_lightType == LightType::Directional)
	{
		AddLight(m_directionalLight);
	}
	else if (m_lightType == LightType::Spot)
	{
		AddLight(m_spotLight);
	}
	else if (m_lightType == LightType::Point)
	{
		AddLight(m_pointLight);
	}
}
#endif // _DEBUG

