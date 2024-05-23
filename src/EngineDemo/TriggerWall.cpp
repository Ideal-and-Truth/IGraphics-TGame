#include "TriggerWall.h"
#include "RigidBody.h"
#include "BoxCollider.h"

TriggerWall::TriggerWall()
{

}

TriggerWall::~TriggerWall()
{

}

void TriggerWall::Initailize()
{
	__super::Initailize();
	// AddComponent<Truth::RigidBody>();
	AddComponent<Truth::BoxCollider>(Vector3{ 20.0f, 20.0f, 2.0f }, true);
	SetPosition(Vector3(0.0f, 20.0f, 10.0f));
}
