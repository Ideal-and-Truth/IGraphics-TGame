#include "BulletMove.h"
#include "Component.h"
#include "Entity.h"
#include "RigidBody.h"

BOOST_CLASS_EXPORT_IMPLEMENT(BulletMove)

BulletMove::BulletMove()
{
	m_name = "BulletMove";
}

BulletMove::~BulletMove()
{

}

void BulletMove::Start()
{
	auto r = m_owner.lock()->GetComponent<Truth::RigidBody>().lock().get();
	Vector3 power(0.0f, 100000.0f, 0.0f);
	r->AddImpulse(power);
}

void BulletMove::Update()
{

}

void BulletMove::OnCollisionEnter(Truth::Collider* _other)
{
	//m_managers.lock()->Scene()->m_currentScene->DeleteEntity(m_owner.lock());
}
