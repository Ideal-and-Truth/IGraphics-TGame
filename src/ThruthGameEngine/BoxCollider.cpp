#include "BoxCollider.h"
#include "RigidBody.h"
#include "Component.h"
#include "Entity.h"

Truth::BoxCollider::BoxCollider(std::shared_ptr<Managers> _managers)
	: Collider(_managers)
	, m_size{1.0f, 1.0f, 1.0f}
{
	m_collider = CreateCollider(ColliderShape::BOX, std::vector<float>{ 1.0f });
	auto r = m_owner.lock()->GetComponent<RigidBody>();
	if (r.expired())
	{
		m_body = GetDefaultStatic();
	}
	m_managers.lock()->Physics()->AddScene(m_body);
}

Truth::BoxCollider::~BoxCollider()
{
	m_collider->release();
}