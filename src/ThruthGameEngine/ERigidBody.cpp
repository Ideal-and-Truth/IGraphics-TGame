#include "ERigidBody.h"
#include "Transform.h"
#include "Entity.h"

Truth::ERigidBody::ERigidBody()
	: m_mass(1.0f)
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
}

Truth::ERigidBody::~ERigidBody()
{

}

void Truth::ERigidBody::Awake()
{
}
