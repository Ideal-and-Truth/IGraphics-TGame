#include "CapsuleCollider.h"

Truth::CapsuleCollider::CapsuleCollider(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner)
	: Collider(_managers, _owner)
	, m_radius(0.5f)
	, m_height(1.0f)
{

}

Truth::CapsuleCollider::~CapsuleCollider()
{

}
