#include "TestComponent.h"
#include "Managers.h"

Truth::TestComponent::TestComponent()
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
	EventBind<TestComponent>("Update", &TestComponent::Update);
}

void Truth::TestComponent::Update(std::any _p)
{
}

void Truth::TestComponent::QUP(std::any _p)
{
	DEBUG_PRINT("Event q up\n");
	EventPublish("Change Scene", std::string("test2"));
}

void Truth::TestComponent::QDOWN(std::any _p)
{
}
