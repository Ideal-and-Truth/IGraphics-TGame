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

void Truth::Collider::SetPosition(Vector3 _pos)
{
	m_center = _pos;
	if (m_body != nullptr)
	{
		m_body->setGlobalPose(physx::PxTransform(_pos.x, _pos.y, _pos.z));
	}
}


