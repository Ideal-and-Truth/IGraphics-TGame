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
	auto r = m_owner.lock()->GetComponent<Truth::RigidBody>().lock().get();
	r->AddImpulse(Vector3(0.0f, 0.0f, -100.0f));
}

void BulletMove::Update(std::any _p)
{
// 	float dt = GetDeltaTime();
// 	float speed = 100;
// 	auto r = m_owner.lock()->GetComponent<Truth::RigidBody>();
// 
// 	Vector3 dir = r.lock()->GetLinearVelocity();
// 	dir.x = 0.0f;
// 	dir.z = 0.0f;
// 	float y = dir.y;
// 	if (GetKey(KEY::UP))
// 	{
// 		dir.z += 1.0f;
// 	}
// 	if (GetKey(KEY::DOWN))
// 	{
// 		dir.z -= 1.0f;
// 	}
// 	if (GetKey(KEY::LEFT))
// 	{
// 		dir.x -= 1.0f;
// 	}
// 	if (GetKey(KEY::RIGHT))
// 	{
// 		dir.x += 1.0f;
// 	}
// 	if (GetKeyDown(KEY::Q))
// 	{
// 		r.lock()->AddImpulse(Vector3(0.0f, 1000.0f, 0.0f));
// 	}
// 
// 	dir.Normalize(dir);
// 	dir *= dt * speed * 10;
// 
// 	dir.y = y;
// 
// 	r.lock()->SetLinearVelocity(dir);
}
