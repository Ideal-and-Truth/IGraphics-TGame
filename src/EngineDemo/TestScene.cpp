#include "TestScene.h"
#include "TestEntity.h"
#include "DefalutCamera.h"

TestScene::TestScene(std::shared_ptr<Truth::Managers> _managers)
	: Truth::Scene(_managers)
{
}

TestScene::~TestScene()
{
}

void TestScene::Enter()
{
	DEBUG_PRINT("%s", "Enter Test Scene\n");
	CreateEntity(std::make_shared<Truth::TestEntity>());
	CreateEntity(std::make_shared<Truth::DefalutCamera>());
}

void TestScene::Awake()
{
}

void TestScene::Exit()
{
	ClearEntity();
	DEBUG_PRINT("%s", "Exit Test Scene\n");
}
