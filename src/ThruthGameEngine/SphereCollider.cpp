#include "SphereCollider.h"
#include "Entity.h"
#include "RigidBody.h"

Truth::SphereCollider::SphereCollider(bool _isTrigger)
	: Collider(_isTrigger)
	, m_radius(0.5f)
{
	m_name = "Sphere Collider";

}

Truth::SphereCollider::SphereCollider(float _radius, bool _isTrigger /*= true*/)
	: Collider(_isTrigger)
{
	m_name = "Sphere Collider";

	SetRadius(_radius);
}

Truth::SphereCollider::SphereCollider(Vector3 _pos, float _radius, bool _isTrigger /*= true*/)
	: Collider(_pos, _isTrigger)
{
	m_name = "Sphere Collider";

	SetRadius(_radius);
}


Truth::SphereCollider::~SphereCollider()
{

}

void Truth::SphereCollider::SetRadius(float _radius)
{
	m_radius = _radius;
	if (m_body != nullptr)
	{
		m_body->detachShape(*m_collider);
		m_collider->release();

		m_collider = CreateCollider(ColliderShape::SPHERE, std::vector<float>{ m_radius });
		m_body->attachShape(*m_collider);
	}
}

void Truth::SphereCollider::Awake()
{
// 	auto r = m_owner.lock()->GetComponent<RigidBody>();
// 	if (r.expired())
// 	{
// 		m_body = GetDefaultDynamic();
// 	}
// 	else
// 	{
// 		m_body = r.lock()->m_body;
// 	}
// 	SetPosition(m_center);
// 	m_body->attachShape(*m_collider);
// 
// 	if (!r.expired())
// 	{
// 		r.lock()->InitalizeMassAndInertia();
// 	}
// 	m_managers.lock()->Physics()->AddScene(m_body);

}

void Truth::SphereCollider::Initalize()
{
	m_collider = CreateCollider(ColliderShape::SPHERE, std::vector<float>{ m_radius});

	m_collider->userData = this;

	SetUpFiltering(m_owner.lock()->m_layer);

	m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
	m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
}
