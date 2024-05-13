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
	AddEntity(std::make_shared<Truth::TestEntity>());
	AddEntity(std::make_shared<Truth::DefalutCamera>());
	
	DEBUG_PRINT(Truth::TestEntity::StaticTypeInfo().Dump(m_entities.front().get()).c_str());
}

void TestScene::Awake()
{

}

void TestScene::Exit()
{
	ClearEntity();
	DEBUG_PRINT("%s", "Exit Test Scene\n");
}
