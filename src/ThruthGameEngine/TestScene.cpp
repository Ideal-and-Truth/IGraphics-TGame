#include "TestScene.h"
#include "TestEntity.h"

TestScene::TestScene(std::shared_ptr<Managers> _managers)
	: Scene(_managers)
{
}

TestScene::~TestScene()
{
}

void TestScene::Enter()
{
	DEBUG_PRINT("%s", "Enter Test Scene\n");
	AddEntity<TestEntity>();

	DEBUG_PRINT(TestEntity::StaticTypeInfo().Dump(m_entities.front().get()).c_str());
}

void TestScene::Awake()
{

}

void TestScene::Exit()
{
	ClearEntity();
	DEBUG_PRINT("%s", "Exit Test Scene\n");
}
