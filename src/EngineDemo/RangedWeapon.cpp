#include "RangedWeapon.h"
#include "Transform.h"
#include "RigidBody.h"
#include "SphereCollider.h"
#include "Mesh.h"
#include "BulletMove.h"

BOOST_CLASS_EXPORT_IMPLEMENT(RangedWeapon)

RangedWeapon::RangedWeapon()
{

}

RangedWeapon::~RangedWeapon()
{

}

void RangedWeapon::Awake()
{
	
}

void RangedWeapon::Update()
{
	if (GetKeyDown(KEY::SPACE))
	{
		std::shared_ptr<Truth::Entity> bullet = std::make_shared<Truth::Entity>(m_managers.lock());
		bullet->m_layer = 1;
		bullet->AddComponent<BulletMove>();
		bullet->AddComponent<Truth::RigidBody>();
		bullet->AddComponent<Truth::SphereCollider>(false);
		bullet->AddComponent<Truth::Mesh>(L"DebugObject/debugSphere");
		bullet->m_name = "bullet";
		m_managers.lock()->Scene()->m_currentScene->CreateEntity(bullet);
		bullet->Initailize();
 		bullet->SetPosition(m_owner.lock()->m_transform->m_worldPosition);
 		bullet->SetScale(Vector3(4.0f, 4.0f, 4.0f));
	}
}
