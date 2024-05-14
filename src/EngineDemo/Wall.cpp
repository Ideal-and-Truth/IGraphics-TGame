#include "Wall.h"
#include "BoxCollider.h"
#include "RigidBody.h"

Wall::Wall()
{

}

Wall::~Wall()
{

}

void Wall::Initailize()
{
	__super::Initailize();
	AddComponent<Truth::BoxCollider>();
	AddComponent<Truth::RigidBody>();
}
