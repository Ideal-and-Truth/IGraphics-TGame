#include "Controller.h"
#include "PhysicsManager.h"
#include "MathConverter.h"
#include "RigidBody.h"
#include "Entity.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Controller)

Truth::Controller::Controller()
	: m_contorller(nullptr)
	, m_isJumping(true)
	, m_jumpStrength(30.0f)
	, m_moveSpeed(10.0f)
	, m_gravity(-100.0f)
	, m_velocity(0.0f)
	, m_jumpVelocity(0.0f)
	, m_minmumDistance(0.0001f)
	, m_flag(0)
	, m_rigidbody(nullptr)
{
	m_name = "TestController";
}

Truth::Controller::~Controller()
{

}

void Truth::Controller::Initalize()
{
	m_contorller = m_managers.lock()->Physics()->CreatePlayerController();
	m_rigidbody = std::make_shared<RigidBody>();

	m_rigidbody->m_transform = m_owner.lock()->GetComponent<Transform>();
	m_rigidbody->m_owner = m_owner.lock();

	m_rigidbody->m_body = m_contorller->getActor();
	m_contorller->getActor()->userData = m_rigidbody.get();
}

void Truth::Controller::Start()
{
	m_managers.lock()->Physics()->AddScene(m_contorller->getActor());
}

void Truth::Controller::Update()
{
	Vector3 disp = Vector3(0.0f, 0.0f, 0.0f);

	UpdateVelocity();
	UpdateMovement(disp);
	UpdateJump(disp);

	auto flag = m_contorller->move(MathConverter::Convert(disp), m_minmumDistance, m_velocity, physx::PxControllerFilters());

	if (flag & physx::PxControllerCollisionFlag::eCOLLISION_DOWN) 
	{
		m_isJumping = false;
	}
}

uint32 Truth::Controller::Move(Vector3& _disp)
{
	m_flag |= 
		static_cast<uint32>(
			m_contorller->move
		(
			MathConverter::Convert(_disp),
			m_minmumDistance,
			m_velocity, 
			physx::PxControllerFilters()
		));

	return m_flag;
}

bool Truth::Controller::SetPosition(Vector3& _disp)
{
	return m_contorller->setPosition(physx::PxExtendedVec3(_disp.x , _disp.y, _disp.z));
}

bool Truth::Controller::IsCollisionDown()
{
	return static_cast<physx::PxControllerCollisionFlag::Enum>(m_flag) & physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
}

void Truth::Controller::UpdateVelocity()
{
	m_velocity = m_moveSpeed * GetDeltaTime();
}

void Truth::Controller::UpdateMovement(Vector3& _disp)
{
	if (GetKey(KEY::W))
	{
		_disp += Vector3(1.0f, 0.0f, 0.0f) * m_velocity;
	}
	if (GetKey(KEY::A))
	{
		_disp += Vector3(0.0f, 0.0f, 1.0f) * m_velocity;
	}
	if (GetKey(KEY::S))
	{
		_disp += Vector3(-1.0f, 0.0f, 0.0f) * m_velocity;
	}
	if (GetKey(KEY::D))
	{
		_disp += Vector3(0.0f, 0.0f, -1.0f) * m_velocity;
	}
}

void Truth::Controller::UpdateGravity(Vector3& _disp)
{
	_disp += Vector3(0.0f, m_gravity, 0.0f) * GetDeltaTime();
}

void Truth::Controller::UpdateJump(Vector3& _disp)
{
	if (!m_isJumping && GetKeyDown(KEY::SPACE))
	{
		m_isJumping = true;
		m_jumpVelocity = m_jumpStrength;
	}

	if (m_isJumping)
	{
		m_jumpVelocity += m_gravity * GetDeltaTime();
	}
	else {
		m_jumpVelocity = 0.0f;
	}


	_disp += Vector3(0.0f, m_jumpVelocity * GetDeltaTime(), 0.0f);
}
