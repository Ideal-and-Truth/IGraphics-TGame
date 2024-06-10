#include "CapsuleCollider.h"
#include "Entity.h"
#include "RigidBody.h"
#include "PhysicsManager.h"

Truth::CapsuleCollider::CapsuleCollider(bool _isTrigger)
	: Collider(_isTrigger)
	, m_radius(0.5f)
	, m_height(1.0f)
{

}

Truth::CapsuleCollider::CapsuleCollider(float _radius, float _height, bool _isTrigger)
	: Collider(_isTrigger)
{
	SetRadius(_radius);
	SetHeight(_height);
}

Truth::CapsuleCollider::CapsuleCollider(Vector3 _pos, float _radius, float _height, bool _isTrigger)
	: Collider(_pos, _isTrigger)
{
	SetRadius(_radius);
	SetHeight(_height);
}


Truth::CapsuleCollider::~CapsuleCollider()
{

}

void Truth::CapsuleCollider::SetRadius(float _radius)
{
	m_radius = _radius;
	if (m_body != nullptr)
	{
		m_body->detachShape(*m_collider);
		m_collider->release();

		m_collider = CreateCollider(ColliderShape::CAPSULE, std::vector<float>{ m_radius, m_height });
		m_body->attachShape(*m_collider);
	}
}

void Truth::CapsuleCollider::SetHeight(float _height)
{
	m_height = _height;
	if (m_body != nullptr)
	{
		m_body->detachShape(*m_collider);
		m_collider->release();

		m_collider = CreateCollider(ColliderShape::CAPSULE, std::vector<float>{ m_radius, m_height });
		m_body->attachShape(*m_collider);
	}
}

void Truth::CapsuleCollider::Awake()
{
}


void Truth::CapsuleCollider::Initalize()
{
	m_collider = CreateCollider(ColliderShape::CAPSULE, std::vector<float>{ m_radius, m_height });

	m_collider->userData = this;

	SetUpFiltering(m_owner.lock()->m_layer);

	m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
	m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
}