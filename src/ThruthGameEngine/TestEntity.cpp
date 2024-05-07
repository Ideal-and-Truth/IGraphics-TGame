#include "TestEntity.h"
#include "TestComponent.h"
#include "Transform.h"
#include "Mesh.h"
#include "Managers.h"
#include "GraphicsManager.h"

Truth::TestEntity::TestEntity()
{
	m_test = std::make_shared<int>(7);
	m_intest.push_back(1);
	m_intest.push_back(2);
	m_intest.push_back(3);


}

Truth::TestEntity::~TestEntity()
{
	int a = 1;
}

void Truth::TestEntity::Initailize()
{
	DEBUG_PRINT("Entity Start : test enttity\n");
	__super::Initailize();
	AddComponent<TestComponent>();
	auto mesh = AddComponent<Mesh>();
	mesh->SetMesh(L"CatwalkWalkForward3/CatwalkWalkForward3");
	mesh->SetRenderable(true);
}
