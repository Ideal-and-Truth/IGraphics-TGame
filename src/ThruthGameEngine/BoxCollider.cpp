#include "BoxCollider.h"
#include "RigidBody.h"
#include "Component.h"
#include "Entity.h"
#include "PhysicsManager.h"

Truth::BoxCollider::BoxCollider(bool _isTrigger)
	: Collider(_isTrigger)
	, m_size{ 1.0f, 1.0f, 1.0f }
{
}


Truth::BoxCollider::BoxCollider(Vector3 _pos, Vector3 _size, bool _isTrigger)
	: Collider(_pos, _isTrigger)
{
	SetSize(_size);
	SetPosition(m_center);
}

Truth::BoxCollider::BoxCollider(Vector3 _size, bool _isTrigger)
	: Collider(_isTrigger)
{
	SetSize(_size);
}

Truth::BoxCollider::~BoxCollider()
{
	if (m_collider != nullptr)
	{
		m_collider->release();
		m_collider = nullptr;
	}
}

void Truth::BoxCollider::SetSize(Vector3 _size)
{
	m_size = _size;
	if (m_body != nullptr)
	{
		m_body->detachShape(*m_collider);
		m_collider->release();

		m_collider = CreateCollider(ColliderShape::BOX, std::vector<float>{ m_size.x, m_size.y, m_size.z });
		m_body->attachShape(*m_collider);
	}
}

void Truth::BoxCollider::Awake()
{
	m_collider = CreateCollider(ColliderShape::BOX, std::vector<float>{ m_size.x, m_size.y, m_size.z });

	physx::PxFilterData filterData;
	filterData.word0 = 0;
	filterData.word1 = 0;

	m_collider->setSimulationFilterData(filterData);

	m_collider->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
	m_collider->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);

	auto f = m_collider->getSimulationFilterData();

	auto r = m_owner.lock()->GetComponent<RigidBody>();
	if (r.expired())
	{
		m_body = GetDefaultDynamic();
	}
	else
	{
		m_body = r.lock()->m_body;
	}
	SetPosition(m_center);
	m_body->attachShape(*m_collider);

	if (!r.expired())
	{
		r.lock()->UpdateMassAndInertia();
	}

	m_managers.lock()->Physics()->AddScene(m_body);
}

void Truth::BoxCollider::Start()
{
}
