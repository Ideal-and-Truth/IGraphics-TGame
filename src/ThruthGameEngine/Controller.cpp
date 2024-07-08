#include "Controller.h"
#include "PhysicsManager.h"
#include "MathConverter.h"
#include "RigidBody.h"
#include "Entity.h"
#include "Transform.h"
#include "Collider.h"
#include "CapsuleCollider.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Controller)

Truth::Controller::Controller()
	: m_controller(nullptr)
	, m_minmumDistance(0.0001f)
	, m_flag(0)
	, m_rigidbody(nullptr)
	, m_height(2.0f)
	, m_contactOffset(0.05f)
	, m_climbingmode(1)
	, m_stepOffset(1.0f)
	, m_radius(1.0f)
	, m_upDirection(0.0f, 1.0f, 0.0f)
	, m_material(1.0f, 1.0f, 0.05f)
{
	m_name = "TestController";
}

Truth::Controller::~Controller()
{

}

/// <summary>
/// 초기화
/// </summary>
void Truth::Controller::Initalize()
{

}

void Truth::Controller::Awake()
{
	physx::PxCapsuleControllerDesc decs;
	decs.height = m_height;
	decs.contactOffset = m_contactOffset;
	decs.stepOffset = m_stepOffset;
	decs.radius = m_radius;
	decs.upDirection = MathConverter::Convert(m_upDirection);
	decs.material = m_managers.lock()->Physics()->CreateMaterial(m_material);
	decs.climbingMode = static_cast<physx::PxCapsuleClimbingMode::Enum>(m_climbingmode);
	decs.position = MathConverter::ConvertEx(m_owner.lock()->GetPosition());

	m_controller = m_managers.lock()->Physics()->CreatePlayerController(decs);

	// create rigidbody to access physx body
	m_rigidbody = std::make_shared<RigidBody>();

	m_rigidbody->m_transform = m_owner.lock()->GetComponent<Transform>();
	m_rigidbody->m_owner = m_owner.lock();
	m_rigidbody->m_isController = true;
	m_rigidbody->m_controller = m_controller;

	m_rigidbody->m_body = m_controller->getActor();
	m_controller->getActor()->userData = m_rigidbody.get();

	// create collider to access physx shape
	m_collider = std::make_shared<CapsuleCollider>();
	m_collider->m_transform = m_owner.lock()->GetComponent<Transform>();
	m_collider->m_owner = m_owner.lock();

	m_collider->m_body = m_controller->getActor();

	uint32 nbs = m_controller->getActor()->getNbShapes();
	physx::PxShape** tempShapes = new physx::PxShape*[nbs];
	m_controller->getActor()->getShapes(tempShapes, nbs);
	m_collider->m_collider = tempShapes[0];

	m_collider->m_collider->userData = m_collider.get();

	delete[] tempShapes;
}

/// <summary>
/// Controller의 Actor를 Scene에 등록함
/// </summary>
void Truth::Controller::Start()
{
	m_managers.lock()->Physics()->AddScene(m_controller->getActor());
}

/// <summary>
/// 움직임 함수
/// </summary>
/// <param name="_disp">속도</param>
void Truth::Controller::Move(Vector3& _disp)
{
	m_flag |= 
		static_cast<uint32>(
			m_controller->move
		(
			MathConverter::Convert(_disp),
			m_minmumDistance,
			1.0f / 60.0f, 
			physx::PxControllerFilters()
		));
}

/// <summary>
/// 고정 위치로 이동 함수
/// </summary>
/// <param name="_disp">위치</param>
/// <returns>성공 여부</returns>
bool Truth::Controller::SetPosition(Vector3& _disp)
{
	return m_controller->setPosition(physx::PxExtendedVec3(_disp.x , _disp.y, _disp.z));
}

/// <summary>
/// 회전 적용
/// </summary>
void Truth::Controller::SetRotation(Quaternion& _val)
{
	GetRigidbody()->SetRotation(_val);
}

/// <summary>
/// Collider의 아래에 충돌이 있는지
/// </summary>
/// <returns>충돌 여부</returns>
bool Truth::Controller::IsCollisionDown()
{
	return static_cast<physx::PxControllerCollisionFlag::Enum>(m_flag) & physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
}
/// <summary>
/// Collider의 위에 충돌이 있는지
/// </summary>
/// <returns>충돌 여부</returns>
bool Truth::Controller::IsCollisionUp()
{
	return static_cast<physx::PxControllerCollisionFlag::Enum>(m_flag) & physx::PxControllerCollisionFlag::eCOLLISION_UP;
}
/// <summary>
/// Collider의 옆에 충돌이 있는지
/// </summary>
/// <returns>충돌 여부</returns>
bool Truth::Controller::IsCollisionSide()
{
	return static_cast<physx::PxControllerCollisionFlag::Enum>(m_flag) & physx::PxControllerCollisionFlag::eCOLLISION_SIDES;
}
