#include "TestComponent.h"
#include "Managers.h"

TestComponent::TestComponent()
{
	m_canMultiple = true;
	m_name = typeid(*this).name();
	m_testInt = 10;
}

TestComponent::~TestComponent()
{
}

void TestComponent::Awake()
{
	EventBind<TestComponent>("Q_UP", &TestComponent::QUP);
	EventBind<TestComponent>("Q_DOWN", &TestComponent::QDOWN);
	EventBind<TestComponent>("Update", &TestComponent::Update);
}

void TestComponent::Update(std::any _p)
{
}

void TestComponent::QUP(std::any _p)
{
	DEBUG_PRINT("Event q up\n");
	EventPublish("Change Scene", std::string("test2"));
}

void TestComponent::QDOWN(std::any _p)
{
}
