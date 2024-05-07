#include "Component.h"
#include "TimeManager.h"
#include "EventManager.h"

Truth::Component::Component()
	: m_owner()
	, m_canMultiple(false)
	, m_managers()
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

float4 Truth::Component::GetDeltaTime()
{
	return m_managers.lock()->Time()->GetDT();
}

float4 Truth::Component::GetFixedDeltaTime()
{
	return m_managers.lock()->Time()->GetFDT();
}

void Truth::Component::EventPublish(std::string _name, std::any _param, float4 _delay)
{
	m_managers.lock()->Event()->PublishEvent(_name, _param, _delay);
}
