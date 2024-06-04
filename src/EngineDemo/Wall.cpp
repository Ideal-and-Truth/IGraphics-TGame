#include "Wall.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "Mesh.h"
#include "TestMovement.h"

Wall::Wall()
{
	m_name = "Wall";
	m_layer = 1;
}

Wall::~Wall()
{

}

void Wall::Initailize()
{
	__super::Initailize();
	AddComponent<Truth::RigidBody>();
	AddComponent<Truth::BoxCollider>(Vector3{20.0f, 20.0f, 2.0f}, false);
	AddComponent<Truth::Mesh>(L"debugCube/debugCube");
	// AddComponent<TestMovement>();
	SetPosition(Vector3(0.0f, 20.0f, 0.0f));
	SetScale(Vector3(40.0f, 40.0f, 4.0f));
	// AddComponent<Truth::Mesh>();
}
