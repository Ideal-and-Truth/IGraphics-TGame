#include "TestFollwer.h"
#include "Controller.h"
#include "Entity.h"

BOOST_CLASS_EXPORT_IMPLEMENT(TestFollwer)

TestFollwer::TestFollwer()
{

}

TestFollwer::~TestFollwer()
{

}

void TestFollwer::Update()
{
	if (!m_controller.expired() && !m_player.expired())
	{
		Vector3 pos = GetPosition();
		Vector3 target = m_managers.lock()->Scene()->m_currentScene->FindPath(
			pos,
			m_player.lock()->GetWorldPosition(),
			GetScale()
		);

		Vector3 dir = target - pos;
		dir.y = 0.0f;
		dir.Normalize(dir);
		dir.y = -100.0f;
		dir *= GetDeltaTime() * 30;
		m_controller.lock()->Move(dir);
	}
}

void TestFollwer::Awake()
{
	m_controller = m_owner.lock()->GetComponent<Truth::Controller>();
	m_player = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
}

