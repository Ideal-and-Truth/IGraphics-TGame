#include "Component.h"
#include "Managers.h"
#include "TimeManager.h"
#include "EventManager.h"

Component::Component()
	: m_owner()
	, m_canMultiple(false)
	, m_managers()
{
	m_name = typeid(*this).name();
}

Component::~Component()
{
	if (!m_managers.expired())
	{
		m_managers.lock()->Event()->RemoveListener(this);
	}
}

float4 Component::GetDeltaTime()
{
	return m_managers.lock()->Time()->GetDT();
}

float4 Component::GetFixedDeltaTime()
{
	return m_managers.lock()->Time()->GetFDT();
}

void Component::EventPublish(std::string _name, std::any _param, float4 _delay)
{
	m_managers.lock()->Event()->PublishEvent(_name, _param, _delay);
}
