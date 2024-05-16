#include "RigidBody.h"
#include "Transform.h"
#include "Entity.h"
#include "PhysicsManager.h"
#include "Collider.h"
#include "MathConverter.h"

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

void Truth::RigidBody::Initalize()
{
	m_body = m_managers.lock()->Physics()->CreateDefaultRigidDynamic();
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
	m_mass = m_body->getMass();
}

void Truth::RigidBody::AddImpulse(Vector3 _force)
{
	m_body->addForce(MathConverter::Convert(_force * m_mass), physx::PxForceMode::eIMPULSE);
}

void Truth::RigidBody::SetLinearVelocity(Vector3 _val)
{
	m_body->setLinearVelocity(MathConverter::Convert(_val));
}

DirectX::SimpleMath::Vector3 Truth::RigidBody::GetLinearVelocity() const
{
	return MathConverter::Convert(m_body->getLinearVelocity());
}

void Truth::RigidBody::Awake()
{
	m_transform = m_owner.lock()->GetComponent<Transform>();
	// m_managers.lock()->Physics()->AddScene(m_body);
}

// void Truth::RigidBody::Start()
// {
// 
// }

