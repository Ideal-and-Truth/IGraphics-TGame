#include "TestScene.h"
#include "TestEntity.h"
#include "DefalutCamera.h"
#include "Wall.h"
#include "Bullet.h"
#include "BulletShooter.h"

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
	CreateEntity(std::make_shared<Wall>());
	// CreateEntity(std::make_shared<Bullet>());
	CreateEntity(std::make_shared<BulletShooter>());
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
