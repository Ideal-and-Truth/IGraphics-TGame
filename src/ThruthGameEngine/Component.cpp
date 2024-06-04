#include "Component.h"
#include "TimeManager.h"
#include "EventManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Entity.h"
#include "Transform.h"

Truth::Component::Component()
	: m_canMultiple(false)
	, m_managers()
	, m_owner()
{
	m_name = typeid(*this).name();
}


Truth::Component::~Component()
{
	if (!m_managers.expired())
	{
		m_managers.lock()->Event()->RemoveListener(this);
	}
}

void Truth::Component::Translate(Vector3& _val)
{
	m_owner.lock()->m_transform->Translate(_val);
}

void Truth::Component::SetPosition(Vector3& _val)
{
	m_owner.lock()->m_transform->SetPosition(_val);
}

void Truth::Component::SetRotation(Quaternion& _val)
{
	m_owner.lock()->m_transform->SetRotate(_val);
}
