#include "BoxCollider.h"
#include "RigidBody.h"
#include "Component.h"
#include "Entity.h"

Truth::BoxCollider::BoxCollider(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner)
	: Collider(_managers, _owner)
	, m_size{ 1.0f, 1.0f, 1.0f }
{
	m_collider = CreateCollider(ColliderShape::BOX, std::vector<float>{ 1.0f });
	auto r = m_owner.lock()->GetComponent<RigidBody>();
	if (r.expired())
	{
		m_body = GetDefaultDynamic();
	}
	else
	{
		m_body = r.lock()->m_body;
	}

	m_body->attachShape(*m_collider);

	m_managers.lock()->Physics()->AddScene(m_body);
}


Truth::BoxCollider::~BoxCollider()
{
	m_collider->release();
}