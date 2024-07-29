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
	, m_position(Vector3(0.f, 0.f, 0.f))
{
	m_name = "Light";
}

Truth::DirectionLight::~DirectionLight()
{
}

void Truth::DirectionLight::SetLight()
{
	m_directionalLight = m_managers.lock()->Graphics()->CreateDirectionalLight();
}

void Truth::DirectionLight::Initalize()
{
	SetLight();
}

void Truth::DirectionLight::ApplyTransform()
{
	m_directionalLight->SetDirection(m_owner.lock().get()->m_transform->m_globalTM.Forward());
}

#ifdef EDITOR_MODE
void Truth::DirectionLight::EditorSetValue()
{
	SetLight();
}
#endif // _DEBUG

