#include "RigidBody.h"
#include "Transform.h"
#include "Entity.h"
#include "PhysicsManager.h"
#include "Collider.h"
#include "MathConverter.h"
#include "MathConverter.h"

Truth::RigidBody::RigidBody()
	: Component()
	, m_mass(1.0f)
	, m_drag(0.0f)
	, m_angularDrag(0.05f)
	, m_useGravity(true)
	, m_isKinematic(false)
	, m_speed(0.0f)
	, m_velocity{ 0.0f, 0.0f, 0.0f }
	, m_inertiaTensor{ 1.0f,1.0f,1.0f }
	, m_inertiaTensorRotation{ 0.0f, 0.0f, 0.0f }
	, m_localMassCenter{ 0.0f, 0.0f, 0.0f }
	, m_worldMassCenter{ 0.0f, 0.0f, 0.0f }
	, m_isChanged(true)
	, m_freezePosition{ false, false , false }
	, m_freezeRotation{ false, false , false }
	, m_body(nullptr)
{
	m_canMultiple = false;
	m_name = typeid(*this).name();
	m_name = "Rigidbody";
}

Truth::RigidBody::~RigidBody()
{
	m_body->release();
}

void Truth::RigidBody::Initalize()
{
	m_body = m_managers.lock()->Physics()->CreateDefaultRigidDynamic();
	m_transform = m_owner.lock()->GetComponent<Transform>();
}

void Truth::RigidBody::Start()
{

}

void Truth::RigidBody::InitalizeMassAndInertia()
{
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

void Truth::RigidBody::Update()
{

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
	auto collider = m_owner.lock()->GetComponents<Truth::Collider>();

	for (auto& c : collider)
	{
		m_body->attachShape(*(c.lock()->m_collider));
		physx::PxTransform t(MathConverter::Convert(m_owner.lock()->GetPosition()));
		m_body->setGlobalPose(t);
		physx::PxRigidBodyExt::updateMassAndInertia(*m_body, 10.0f);
	}

	m_body->setAngularDamping(5.0f);

	m_managers.lock()->Physics()->AddScene(m_body);

	m_mass = m_body->getMass();
	m_drag = m_body->getLinearDamping();
	m_angularDrag = m_body->getAngularDamping();

	auto actorFlag = m_body->getActorFlags();
	m_useGravity = !actorFlag.isSet(physx::PxActorFlag::eDISABLE_GRAVITY);
	m_isKinematic = actorFlag.isSet(physx::PxActorFlag::eDISABLE_SIMULATION);

	auto rigidFlag = m_body->getRigidDynamicLockFlags();
	m_freezePosition[0] = rigidFlag.isSet(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X);
	m_freezePosition[1] = rigidFlag.isSet(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y);
	m_freezePosition[2] = rigidFlag.isSet(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z);

	m_freezeRotation[0] = rigidFlag.isSet(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X);
	m_freezeRotation[1] = rigidFlag.isSet(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);
	m_freezeRotation[2] = rigidFlag.isSet(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
}
