#include "TestScene2.h"
#include "TestEntity.h"

Truth::TestScene2::TestScene2(std::shared_ptr<Managers> _managers)
	: Scene(_managers)
{
}

Truth::TestScene2::~TestScene2()
{
}

void Truth::TestScene2::Enter()
{
	DEBUG_PRINT("%s", "Enter Test Scene 2\n");
	AddEntity<TestEntity>();
}

void Truth::TestScene2::Awake()
{

}

void Truth::TestScene2::Exit()
{
	ClearEntity();
	DEBUG_PRINT("%s", "Exit Test Scene 2\n");
}
