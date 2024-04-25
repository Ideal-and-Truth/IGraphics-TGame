#include "TestComponent.h"
#include "Managers.h"

REFLECT_STRUCT_BEGIN(TestComponent)
REFLECT_STRUCT_END()

TestComponent::TestComponent()
{
	m_canMultiple = true;
	m_name = typeid(*this).name();
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
