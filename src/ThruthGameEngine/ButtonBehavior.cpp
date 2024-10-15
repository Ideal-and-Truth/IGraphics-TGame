#include "ButtonBehavior.h"
BOOST_CLASS_EXPORT_IMPLEMENT(Truth::ButtonBehavior)

Truth::ButtonBehavior::ButtonBehavior()
	: m_managers(nullptr)
	, m_UI(nullptr)
	, m_TextUI(nullptr)
	, m_owner(nullptr)
{

}

Truth::ButtonBehavior::~ButtonBehavior()
{

}

void Truth::ButtonBehavior::Initialize(Managers* _managers, UI* _UI, Entity* _owner)
{
	m_managers = _managers;
	m_UI = _UI;
	m_owner = _owner;
}

void Truth::ButtonBehavior::Initialize(Managers* _managers, TextUI* _UI, Entity* _owner)
{
	m_managers = _managers;
	m_TextUI = _UI;
	m_owner = _owner;
}
