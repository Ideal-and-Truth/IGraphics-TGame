#include "TestComponent.h"
#include "Managers.h"

TestComponent::TestComponent()
{
	m_canMultiple = true;
	Managers::Get()->Event()->Subscribe("Q_HOLD", MakeListenerInfo(&TestComponent::EventTestFunc)) ;
}

TestComponent::~TestComponent()
{
}

void TestComponent::Update(float4 _dt)
{
	
}

void TestComponent::Render()
{
}

void TestComponent::LateUpdate(float4 _dt)
{
}

void TestComponent::FiexUpdate(float4 _dt)
{
}

void TestComponent::EventTestFunc(std::shared_ptr<void> _p)
{
	int a = 1;
}
