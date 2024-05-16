#include "Bullet.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "BulletMove.h"

Bullet::Bullet()
{

}

Bullet::~Bullet()
{

}

void Bullet::Initailize()
{
	__super::Initailize();
	AddComponent<BulletMove>();
	AddComponent<Truth::RigidBody>();
	AddComponent<Truth::BoxCollider>(Vector3{ 0.0f, 50.0f, 50.0f }, Vector3{ 1.0f, 1.0f, 1.0f }, false);
}
