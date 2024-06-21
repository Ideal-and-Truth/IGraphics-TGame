#include "ControllerTest.h"
#include "PhysicsManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::ControllerTest)

Truth::ControllerTest::ControllerTest()
{
	m_name = "TestController";
}

Truth::ControllerTest::~ControllerTest()
{

}

void Truth::ControllerTest::Initalize()
{
	m_contorller = m_managers.lock()->Physics()->CreatePlayerController();
}

void Truth::ControllerTest::Start()
{
	m_managers.lock()->Physics()->AddScene(m_contorller->getActor());
}

void Truth::ControllerTest::Update()
{
	float velocity = 10 * GetDeltaTime();
	physx::PxVec3 disp = physx::PxVec3(1.0f, 0.0f, 0.0f) * velocity;
	m_contorller->move(disp, 0.01f, velocity, physx::PxControllerFilters());
}
