#include "TestScene.h"
#include "TestEntity.h"
#include "DefalutCamera.h"

Truth::TestScene::TestScene(std::shared_ptr<Managers> _managers)
	: Scene(_managers)
{
}

Truth::TestScene::~TestScene()
{
}

void Truth::TestScene::Enter()
{
	DEBUG_PRINT("%s", "Enter Test Scene\n");
	AddEntity<TestEntity>();
	AddEntity<DefalutCamera>();
	
	DEBUG_PRINT(TestEntity::StaticTypeInfo().Dump(m_entities.front().get()).c_str());
}

void Truth::TestScene::Awake()
{

}

void Truth::TestScene::Exit()
{
	ClearEntity();
	DEBUG_PRINT("%s", "Exit Test Scene\n");
}
