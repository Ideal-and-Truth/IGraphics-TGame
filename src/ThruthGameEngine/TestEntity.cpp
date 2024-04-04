#include "TestEntity.h"
#include "TestComponent.h"
TestEntity::TestEntity()
{
	AddComponent<TestComponent>();
	AddComponent<TestComponent>();
	AddComponent<TestComponent>();
	AddComponent<TestComponent>();
	// AddComponent<int>();
}

TestEntity::~TestEntity()
{

}

void TestEntity::Update(float4 _dt)
{
	int a = 1;
	for (auto& c : m_multipleComponents)
	{
		for (auto& cc: c.second)
		{
			cc->Update(_dt);
		}
	}

	for (auto& c : m_uniqueComponents)
	{
		c.second->Update(_dt);
	}
}

void TestEntity::Render()
{
}

void TestEntity::LateUpdate(float4 _dt)
{
}

void TestEntity::FixedUpdate(float4 _dt)
{
}
