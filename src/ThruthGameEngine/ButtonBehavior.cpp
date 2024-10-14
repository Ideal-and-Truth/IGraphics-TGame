#include "ButtonBehavior.h"
BOOST_CLASS_EXPORT_IMPLEMENT(Truth::ButtonBehavior)

Truth::ButtonBehavior::ButtonBehavior()
{

}

Truth::ButtonBehavior::~ButtonBehavior()
{

}

void Truth::ButtonBehavior::Initialize(std::shared_ptr<Managers> _managers)
{
	m_managers = _managers;
}
