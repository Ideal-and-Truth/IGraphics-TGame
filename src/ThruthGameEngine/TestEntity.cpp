#include "TestEntity.h"
#include "TestComponent.h"
#include "Transform.h"

REFLECT_STRUCT_BEGIN(TestEntity)
REFLECT_STRUCT_MEMBER(m_ID)
REFLECT_STRUCT_MEMBER(m_name)
REFLECT_STRUCT_MEMBER(m_components)
REFLECT_STRUCT_END()

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

	reflect::TypeDescriptor* typeDesc = reflect::TypeResolver<TestEntity>::Get();
	std::string dump = typeDesc->Dump(this);
	DEBUG_PRINT(dump.c_str());
}
