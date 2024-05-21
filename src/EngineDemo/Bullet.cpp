#include "Bullet.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "BulletMove.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
Bullet::Bullet()
{
	m_layer = 2;
}

Bullet::~Bullet()
{

}

void Bullet::Initailize()
{
	__super::Initailize();
	AddComponent<BulletMove>();
	AddComponent<Truth::RigidBody>();
	AddComponent<Truth::SphereCollider>(2.0f, false);

	SetPosition(Vector3{ 0.0f, 50.0f, 50.0f });
}
