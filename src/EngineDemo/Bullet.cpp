#include "Bullet.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "BulletMove.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "Mesh.h"

Bullet::Bullet()
{
	m_name = "Bullet";
	m_layer = 2;
}

Bullet::~Bullet()
{

}

void Bullet::Initialize()
{
	__super::Initialize();
	AddComponent<BulletMove>();
	AddComponent<Truth::RigidBody>();
	AddComponent<Truth::SphereCollider>(2.0f, false);
	AddComponent<Truth::Mesh>(L"debugCube/debugCube");
	SetPosition(Vector3{ 0.0f, 50.0f, 50.0f });
	SetScale(Vector3(4.0f, 4.0f, 4.0f));
}
