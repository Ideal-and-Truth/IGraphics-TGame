#include "TestScene2.h"
#include "TestEntity.h"

TestScene2::TestScene2(std::shared_ptr<Managers> _managers)
	: Scene(_managers)
{
}

TestScene2::~TestScene2()
{
}

void TestScene2::Enter()
{
	DEBUG_PRINT("%s", "Enter Test Scene 2\n");
	AddEntity<TestEntity>();
}

void TestScene2::Awake()
{

}

void TestScene2::Exit()
{
	ClearEntity();
	DEBUG_PRINT("%s", "Exit Test Scene 2\n");
}
