#include "RigidBody.h"
#include "Transform.h"
#include "Entity.h"
#include "PhysicsManager.h"
#include "Collider.h"

Truth::RigidBody::RigidBody()
	: Component()
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

}

Truth::RigidBody::~RigidBody()
{
	m_body->release();
}

void Truth::RigidBody::FixedUpdate(std::any _p)
{
}

void Truth::RigidBody::FreezePosition(bool _xzy[3])
{

}

void Truth::RigidBody::FreezeRotation(bool _xzy[3])
{

}

void Truth::RigidBody::UpdateMassAndInertia()
{
	physx::PxRigidBodyExt::updateMassAndInertia(*m_body, 10.0f);
}

void Truth::RigidBody::Awake()
{
	m_body = m_managers.lock()->Physics()->CreateDefaultRigidDynamic();

	m_transform = m_owner.lock()->GetComponent<Transform>();
	// m_managers.lock()->Physics()->AddScene(m_body);
}

// void Truth::RigidBody::Start()
// {
// 
// }

