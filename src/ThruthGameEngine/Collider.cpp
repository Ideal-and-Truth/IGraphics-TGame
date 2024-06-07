#include "Collider.h"

Truth::Collider::Collider(bool _isTrigger)
	: Component()
	, m_isTrigger(_isTrigger)
	, m_center{ 0.0f, 0.0f, 0.0f }
	, m_collider(nullptr)
	, m_body(nullptr)
	, m_colliderID(m_colliderIDGenerator++)
{
}

Truth::Collider::Collider(Vector3 _pos, bool _isTrigger /*= true*/)
	: m_center(_pos)
	, m_isTrigger(_isTrigger)
	, m_collider(nullptr)
	, m_body(nullptr)
	, m_colliderID(m_colliderIDGenerator++)
{

}

Truth::Collider::~Collider()
{
}

void Truth::Collider::SetPhysxTransform(Vector3 _pos)
{
	m_center = _pos;
	if (m_body != nullptr)
	{
		physx::PxQuat q;
		q.x = m_rotation.x;
		q.y = m_rotation.y;
		q.z = m_rotation.z;
		q.w = m_rotation.w;
		m_body->setGlobalPose(physx::PxTransform(_pos.x, _pos.y, _pos.z, q));
	}
}

void Truth::Collider::SetPhysxTransform(Vector3 _pos, Quaternion _rot)
{
	m_center = _pos;
	m_rotation = _rot;
	if (m_body != nullptr)
	{
		physx::PxQuat q;
		q.x = m_rotation.x;
		q.y = m_rotation.y;
		q.z = m_rotation.z;
		q.w = m_rotation.w;
		m_body->setGlobalPose(physx::PxTransform(_pos.x, _pos.y, _pos.z, q));
	}
}


