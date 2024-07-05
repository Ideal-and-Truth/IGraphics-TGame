#include "EnemyController.h"
#include "Controller.h"
#include "Enemy.h"

BOOST_CLASS_EXPORT_IMPLEMENT(EnemyController)

EnemyController::EnemyController()
{
	m_name = "EnemyController";
}

EnemyController::~EnemyController()
{

}

void EnemyController::Awake()
{
	m_controller = m_owner.lock()->GetComponent<Truth::Controller>();
	m_enemy = m_owner.lock()->GetComponent<Enemy>();
	m_target = m_enemy.lock().get()->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.lock().get()).Get();
}

void EnemyController::Update()
{
	bool isTargetIn = m_enemy.lock().get()->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.lock().get()).Get();
	
	if (!isTargetIn)
	{
		return;
	}

	FollowTarget();
}

void EnemyController::FollowTarget()
{
	if (!m_controller.expired() && !m_target.expired())
	{
		Vector3 pos = GetPosition();
		Vector3 targetPos = m_managers.lock()->Scene()->m_currentScene->FindPath(
			pos,
			m_target.lock()->GetPosition(),
			GetScale()
		);

		Vector3 dir = targetPos - pos;
		dir.y = 0.0f;
		dir.Normalize(dir);
		dir.y = -100.0f;
		dir *= GetDeltaTime() * 10;
		m_controller.lock()->Move(dir);
	}
}
