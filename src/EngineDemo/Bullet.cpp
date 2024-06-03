#include "Bullet.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "BulletMove.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "Mesh.h"

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
	AddComponent<Truth::Mesh>(L"statue_chronos/SMown_chronos_statue2");
// 	SetPosition(Vector3{ 0.0f, 50.0f, 50.0f });
// 	SetScale(Vector3(2.0f, 2.0f, 2.0f));
}
