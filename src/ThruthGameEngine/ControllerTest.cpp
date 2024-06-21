#include "ControllerTest.h"
#include "PhysicsManager.h"
#include "MathConverter.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::ControllerTest)

Truth::ControllerTest::ControllerTest()
	: m_contorller(nullptr)
	, m_isJumping(true)
	, m_jumpStrength(30.0f)
	, m_moveSpeed(10.0f)
	, m_gravity(-100.0f)
{
	m_name = "TestController";
}

Truth::ControllerTest::~ControllerTest()
{

}

void Truth::ControllerTest::Initalize()
{
	m_contorller = m_managers.lock()->Physics()->CreatePlayerController();
	m_contorller->getActor()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);

	physx::PxShape* shapes[128];
	physx::PxU32 nbShapes = m_contorller->getActor()->getShapes(shapes, 128);
	for (physx::PxU32 i = 0; i < nbShapes; ++i)
	{
		shapes[i]->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	}
}

void Truth::ControllerTest::Start()
{
	m_managers.lock()->Physics()->AddScene(m_contorller->getActor());
}

void Truth::ControllerTest::Update()
{
	Vector3 disp = Vector3(0.0f, 0.0f, 0.0f);

	UpdateVelocity();
	// UpdateGravity(disp);
	UpdateMovement(disp);
	UpdateJump(disp);

	auto flag = m_contorller->move(MathConverter::Convert(disp), 0.0001f, m_velocity, physx::PxControllerFilters());

	if (flag & physx::PxControllerCollisionFlag::eCOLLISION_DOWN) 
	{
		m_isJumping = false;
	}
}

void Truth::ControllerTest::UpdateVelocity()
{
	m_velocity = m_moveSpeed * GetDeltaTime();
}

void Truth::ControllerTest::UpdateMovement(Vector3& _disp)
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

void Truth::ControllerTest::UpdateGravity(Vector3& _disp)
{
	_disp += Vector3(0.0f, m_gravity, 0.0f) * GetDeltaTime();
}

void Truth::ControllerTest::UpdateJump(Vector3& _disp)
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
