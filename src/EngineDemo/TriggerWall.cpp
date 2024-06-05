#include "TriggerWall.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "Mesh.h"
TriggerWall::TriggerWall()
{
	m_name = "TriggerWall";
	m_layer = 1;
}

TriggerWall::~TriggerWall()
{

}

void TriggerWall::Initailize()
{
	__super::Initailize();
	// AddComponent<Truth::RigidBody>();
	AddComponent<Truth::BoxCollider>(Vector3{ 20.0f, 20.0f, 2.0f }, true);
	AddComponent<Truth::Mesh>(L"debugCube/debugCube");
	SetPosition(Vector3(0.0f, 20.0f, 10.0f));
	SetScale(Vector3(40.0f, 40.0f, 4.0f));
}
