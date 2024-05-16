#include "BulletMove.h"
#include "Component.h"
#include "Entity.h"
#include "RigidBody.h"

BulletMove::BulletMove()
{

}

BulletMove::~BulletMove()
{

}

void BulletMove::Start()
{
	EventBind("Update", &BulletMove::Update);
	auto r = m_owner.lock()->GetComponent<Truth::RigidBody>();
	r.lock()->AddForce(Vector3(0.0f, -1000000.0f, -1000000.0f));
}

void BulletMove::Update(std::any _p)
{
}
