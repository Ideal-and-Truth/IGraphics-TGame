#include "TestComponent.h"
#include "Managers.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::TestComponent)

Truth::TestComponent::TestComponent()
	: Component()
{
	m_canMultiple = true;
	m_name = typeid(*this).name();
	m_testInt = 10;
}


Truth::TestComponent::~TestComponent()
{
}

void Truth::TestComponent::Awake()
{
	EventBind<TestComponent>("Q_UP", &TestComponent::QUP);
	EventBind<TestComponent>("Q_DOWN", &TestComponent::QDOWN);
}

void Truth::TestComponent::Update()
{
}

void Truth::TestComponent::QUP(const void* _p)
{
	DEBUG_PRINT("Event q up\n");
}

void Truth::TestComponent::QDOWN(const void* _p)
{
	DEBUG_PRINT("Event q Down\n");
}
