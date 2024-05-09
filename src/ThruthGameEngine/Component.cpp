#include "Component.h"
#include "TimeManager.h"
#include "EventManager.h"

Truth::Component::Component(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner) 
	: m_canMultiple(false)
	, m_managers(_managers)
	, m_owner(_owner)
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
