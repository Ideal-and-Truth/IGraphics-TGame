#include "Shooter.h"
#include "BulletMove.h"
#include "RigidBody.h"
#include "SphereCollider.h"
#include "Mesh.h"
// #include "Bullet.h"

Shooter::Shooter()
{

}

Shooter::~Shooter()
{

}

void Shooter::Awake()
{
}

void Shooter::Update()
{
	if (GetKeyDown(KEY::SPACE))
	{
		std::shared_ptr<Truth::Entity> bullet = std::make_shared<Truth::Entity>(m_managers.lock());
		bullet->AddComponent<BulletMove>();
		bullet->AddComponent<Truth::RigidBody>();
		bullet->AddComponent<Truth::SphereCollider>(2.0f, false);
		bullet->AddComponent<Truth::Mesh>(L"debugCube/debugCube");
		bullet->SetPosition(Vector3{ 0.0f, 50.0f, 50.0f });
		bullet->SetScale(Vector3(4.0f, 4.0f, 4.0f));
		
		m_managers.lock()->Scene()->m_currentScene.lock()->CreateEntity(bullet);
	}
}
