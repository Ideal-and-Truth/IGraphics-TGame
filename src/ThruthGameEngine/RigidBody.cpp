#include "RigidBody.h"
#include "Transform.h"
#include "Entity.h"
#include "PhysicsManager.h"

Truth::RigidBody::RigidBody(std::shared_ptr<Managers> _managers, std::shared_ptr<Entity> _owner)
	: Component(_managers, _owner)
	, m_mass(1.0f)
	, m_drag(0.0f)
	, m_angularDrag(0.05f)
	, m_useGravity(true)
	, m_isKinematic(false)
	, m_speed(0.0f)
	, m_velocity{0.0f, 0.0f, 0.0f}
	, m_inertiaTensor{1.0f,1.0f,1.0f}
	, m_inertiaTensorRotation{0.0f, 0.0f, 0.0f}
	, m_localMassCenter{0.0f, 0.0f, 0.0f}
	, m_worldMassCenter{0.0f, 0.0f, 0.0f}
{
	m_canMultiple = false;
	m_name = typeid(*this).name();
	m_transform = m_owner.lock()->GetComponent<Transform>();

	m_body = m_managers.lock()->Physics()->CreateDefaultRigidDynamic();
}

Truth::RigidBody::~RigidBody()
{
	m_body->release();
}

void Truth::RigidBody::FixedUpdate(std::any _p)
{
	int a = 1;
}

