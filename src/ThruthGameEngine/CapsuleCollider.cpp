#include "CapsuleCollider.h"

Truth::CapsuleCollider::CapsuleCollider(std::shared_ptr<Managers> _managers)
	: Collider(_managers)
	, m_radius(0.5f)
	, m_height(1.0f)
{

}

Truth::CapsuleCollider::~CapsuleCollider()
{

}
