#include "TestScene.h"
#include "TestEntity.h"

TestScene::TestScene()
{

}

TestScene::~TestScene()
{

}

void TestScene::FrontUpdate(float4 _dt)
{

}

void TestScene::BackUpdate(float4 _dt)
{

}

void TestScene::Enter()
{
	AddEntity<TestEntity>();
}

void TestScene::Exit()
{

}
