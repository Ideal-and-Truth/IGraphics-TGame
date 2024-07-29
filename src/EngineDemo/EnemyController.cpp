#include "EnemyController.h"
#include "Controller.h"
#include "Enemy.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(EnemyController)

EnemyController::EnemyController()
	: m_speed(0.f)
	, m_isComeBack(false)
{
	m_name = "EnemyController";
}

EnemyController::~EnemyController()
{

}

void EnemyController::Start()
{
	m_controller = m_owner.lock()->GetComponent<Truth::Controller>();
	m_enemy = m_owner.lock()->GetComponent<Enemy>();
	m_target = m_enemy.lock().get()->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.lock().get()).Get();
	m_homePos = m_owner.lock()->m_transform->m_position;
}

void EnemyController::Update()
{
	bool isTargetIn = m_enemy.lock().get()->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.lock().get()).Get();
	m_speed = m_enemy.lock().get()->GetTypeInfo().GetProperty("speed")->Get<float>(m_enemy.lock().get()).Get();

	if (!isTargetIn)
	{
		ComeBackHome();
		return;
	}

	FollowTarget();
}

void EnemyController::FollowTarget()
{
	if (!m_controller.expired() && !m_target.expired())
	{
		Vector3 pos = m_owner.lock()->GetWorldPosition();
		Vector3 targetPos = m_managers.lock()->Scene()->m_currentScene->FindPath(
			pos,
			m_target.lock()->GetWorldPosition(),
			GetScale()
		);

		Vector3 dir = targetPos - pos;
		float distance = sqrt(pow(dir.x, 2.f) + pow(dir.z, 2.f));
		if (distance > 10.f)
		{
			dir.y = 0.0f;
			dir.Normalize(dir);
			dir.y = -100.0f;
			dir *= GetDeltaTime() * m_speed;
			m_controller.lock()->Move(dir);

		}
		else if (distance < 9.f)
		{
			Vector3 backDir = dir;
			backDir.y = 0.0f;
			backDir.Normalize(backDir);
			backDir.y = -100.0f;
			backDir *= GetDeltaTime() * m_speed;

			backDir.x *= -1.f;
			backDir.z *= -1.f;
			m_controller.lock()->Move(backDir);
		}

		// 적 회전
		dir.y = 0.0f;
		Quaternion lookRot;
		Quaternion::LookRotation(dir, Vector3::Up, lookRot);
		m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, lookRot, 10.f * GetDeltaTime());

	}
}

void EnemyController::ComeBackHome()
{
	if (!m_controller.expired())
	{
		Vector3 pos = m_owner.lock()->GetWorldPosition();
		Vector3 targetPos = m_managers.lock()->Scene()->m_currentScene->FindPath(
			pos,
			m_target.lock()->GetWorldPosition(),
			GetScale()
		);


		Vector3 dir = targetPos - pos;
		Vector3 backDir = m_homePos - pos;

		Quaternion lookRot;
		Quaternion rot = m_owner.lock()->m_transform->m_rotation;


		float distance = sqrt(pow(backDir.x, 2.f) + pow(backDir.z, 2.f));
		if (distance > 1.f)
		{
			m_isComeBack = true;
			backDir.y = 0.0f;
			backDir.Normalize(backDir);
			backDir.y = -100.0f;
			backDir *= GetDeltaTime() * m_speed;
			m_controller.lock()->Move(backDir);
			// 적 회전
			backDir.y = 0.0f;
			Quaternion::LookRotation(backDir, Vector3::Up, lookRot);
			m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, lookRot, 10.f * GetDeltaTime());
		}

		else
		{
			Quaternion::LookRotation(dir, Vector3::Up, lookRot);
			if (m_owner.lock()->m_transform->m_rotation.ToEuler().y > lookRot.ToEuler().y - 0.2f && m_owner.lock()->m_transform->m_rotation.ToEuler().y < lookRot.ToEuler().y + 0.2f)
			{
				m_isComeBack = false;
			}
			if (m_isComeBack)
			{
				// 적 회전
				dir.y = 0.0f;
				m_owner.lock()->m_transform->m_rotation = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_rotation, lookRot, 10.f * GetDeltaTime());
			}
		}


	}
}
