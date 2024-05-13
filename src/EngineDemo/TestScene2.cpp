#include "TestScene2.h"
#include "TestEntity.h"

TestScene2::TestScene2(std::shared_ptr<Truth::Managers> _managers)
	: Truth::Scene(_managers)
{
}

TestScene2::~TestScene2()
{
}

void TestScene2::Enter()
{
	DEBUG_PRINT("%s", "Enter Test Scene 2\n");
	CreateEntity(std::make_shared<Truth::TestEntity>());
}

void TestScene2::Awake()
{

}

void TestScene2::Exit()
{
	ClearEntity();
	DEBUG_PRINT("%s", "Exit Test Scene 2\n");
}
