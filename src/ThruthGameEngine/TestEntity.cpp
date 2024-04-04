#include "TestEntity.h"
#include "TestComponent.h"
TestEntity::TestEntity()
{
	AddComponent<TestComponent>();
	AddComponent<TestComponent>();
	AddComponent<TestComponent>();
	AddComponent<TestComponent>();
}

TestEntity::~TestEntity()
{

}

void TestEntity::Update(float4 _dt)
{
	int a = 1;
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
