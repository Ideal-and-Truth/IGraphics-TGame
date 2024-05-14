#include "Component.h"
#include "TimeManager.h"
#include "EventManager.h"

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
