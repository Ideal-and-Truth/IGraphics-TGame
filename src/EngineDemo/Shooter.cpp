#include "Shooter.h"
#include "BulletMove.h"
#include "RigidBody.h"
#include "SphereCollider.h"
#include "Mesh.h"
// #include "Bullet.h"
BOOST_CLASS_EXPORT_IMPLEMENT(Shooter)

Shooter::Shooter()
{
	m_name = "Shooter";
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
		bullet->m_layer = 1;
		bullet->AddComponent<BulletMove>();
		bullet->AddComponent<Truth::RigidBody>();
		bullet->AddComponent<Truth::SphereCollider>(false);
		bullet->AddComponent<Truth::Mesh>(L"DebugObject/debugSphere");
		bullet->SetPosition(Vector3{ 0.0f, 50.0f, 50.0f });
		bullet->SetScale(Vector3(4.0f, 4.0f, 4.0f));
		bullet->m_name = "bullet";
		m_managers.lock()->Scene()->m_currentScene.lock()->CreateEntity(bullet);
	}
}
