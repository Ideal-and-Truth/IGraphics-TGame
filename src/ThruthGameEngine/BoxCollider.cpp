#include "BoxCollider.h"
#include "PhysicsManager.h"

Truth::BoxCollider::BoxCollider(std::shared_ptr<Managers> _managers)
	: Collider(_managers)
	, m_size{1.0f, 1.0f, 1.0f}
{
	m_collider = m_managers.lock()->Physics()->CreateCollider(ColliderShape::BOX, std::vector<float>{ 1.0f });
}

Truth::BoxCollider::~BoxCollider()
{
}