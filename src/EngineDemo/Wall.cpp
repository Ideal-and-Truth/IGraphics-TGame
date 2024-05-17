#include "Wall.h"
#include "BoxCollider.h"
#include "RigidBody.h"

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
	AddComponent<Truth::BoxCollider>(Vector3{ 0.0f, 20.0f, 0.0f }, Vector3{20.0f, 20.0f, 2.0f}, false);
}
