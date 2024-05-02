#include "TestEntity.h"
#include "TestComponent.h"
#include "ETransform.h"

TestEntity::TestEntity()
{
	m_test = std::make_shared<int>(7);
	m_intest.push_back(1);
	m_intest.push_back(2);
	m_intest.push_back(3);


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
