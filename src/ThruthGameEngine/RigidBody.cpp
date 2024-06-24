#include "RigidBody.h"
#include "Transform.h"
#include "Entity.h"
#include "PhysicsManager.h"
#include "Collider.h"
#include "MathConverter.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::RigidBody)

/// <summary>
/// 생성자
/// </summary>
Truth::RigidBody::RigidBody()
	: Component()
	, m_mass(1000.0f)
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

/// <summary>
/// 소멸자
/// </summary>
Truth::RigidBody::~RigidBody()
{
	if (m_body)
	{
		m_body->release();
	}
}

/// <summary>
/// 초기화
/// body 생성 / transform 연동
/// </summary>
void Truth::RigidBody::Initalize()
{
	m_body = m_managers.lock()->Physics()->CreateDefaultRigidDynamic();
	m_transform = m_owner.lock()->GetComponent<Transform>();
}

/// <summary>
/// 에디터나 인게임에서 이동 한 만큼 이동
/// 단, 해당 컴포넌트가 있는 엔티티의 경우 강제 이동 보다 물리 이동을 추천
/// </summary>
void Truth::RigidBody::ApplyTransform()
{
	// 	Vector3 pos = m_owner.lock()->GetPosition();
	// 	Quaternion rot = m_owner.lock()->GetRotation();
	// 
	// 	m_body->setGlobalPose(physx::PxTransform(
	// 		MathConverter::Convert(pos),
	// 		MathConverter::Convert(rot)
	// 	), false);
}

void Truth::RigidBody::Start()
{
	for (auto& c : m_colliders)
	{
		m_body->attachShape(*(c.lock()->m_collider));
	}
	InitalizeMassAndInertia();
}

void Truth::RigidBody::Destroy()
{

}

void Truth::RigidBody::CalculateMassCenter()
{
	auto massCenter = m_body->getCMassLocalPose();
	Vector3 pos{ 0.0f, 0.0f, 0.0f };
	for (auto& c : m_colliders)
	{
		pos += c.lock()->m_center;
	}
	pos /= static_cast<float>(m_colliders.size());

	m_body->setCMassLocalPose(
		physx::PxTransform(MathConverter::Convert(pos)));
}

void Truth::RigidBody::InitalizeMassAndInertia()
{
	auto mcenter = m_body->getCMassLocalPose();
	physx::PxRigidBodyExt::setMassAndUpdateInertia(
		*m_body,
		physx::PxReal(m_mass),
		&(mcenter.p)
	);
}

void Truth::RigidBody::Update()
{
}

void Truth::RigidBody::AddImpulse(Vector3& _force)
{
	m_body->addForce(MathConverter::Convert(_force), physx::PxForceMode::eIMPULSE);
}

void Truth::RigidBody::SetLinearVelocity(Vector3& _val)
{
	m_body->setLinearVelocity(MathConverter::Convert(_val));
}

DirectX::SimpleMath::Vector3 Truth::RigidBody::GetLinearVelocity() const
{
	return MathConverter::Convert(m_body->getLinearVelocity());
}

void Truth::RigidBody::Awake()
{
	m_colliders = m_owner.lock()->GetComponents<Truth::Collider>();
	Vector3 pos{ 0.0f, 0.0f, 0.0f };

	m_localTM = Matrix::Identity;

	for (auto& c : m_colliders)
	{
		m_localTM *= c.lock()->m_localTM;
	}

	m_body->setMass(m_mass);
	m_body->setLinearDamping(m_drag);
	m_body->setAngularDamping(m_angularDrag);

	m_body->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !m_useGravity);
	m_body->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, m_isKinematic);

	m_body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, m_freezePosition[0]);
	m_body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, m_freezePosition[1]);
	m_body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, m_freezePosition[2]);

	m_body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, m_freezeRotation[0]);
	m_body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, m_freezeRotation[1]);
	m_body->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, m_freezeRotation[2]);

	m_body->userData = this;

	CalculateMassCenter();
	m_globalTM = m_localTM * m_owner.lock()->GetWorldTM();
	m_body->setGlobalPose(MathConverter::Convert(m_globalTM));

	m_managers.lock()->Physics()->AddScene(m_body);
}
