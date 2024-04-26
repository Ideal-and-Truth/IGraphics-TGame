#include "TestEntity.h"
#include "TestComponent.h"
#include "ETransform.h"

TestEntity::TestEntity()
{
}

TestEntity::~TestEntity()
{
	int a = 1;
}

void TestEntity::Initailize()
{
	DEBUG_PRINT("Entity Start : test enttity\n");
	__super::Initailize();
	AddComponent<TestComponent>();
}
