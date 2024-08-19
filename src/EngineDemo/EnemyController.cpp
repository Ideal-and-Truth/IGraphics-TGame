#include "EnemyController.h"
#include "Controller.h"
#include "Enemy.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(EnemyController)

EnemyController::EnemyController()
	: m_speed(0.f)
	, m_isComeBack(false)
	, m_isAttackReady(false)
	, m_isBackStep(false)
	, m_isDead(false)
	, m_canMove(false)
	, m_passingTime(0.f)
	, m_sideMove(1.f)
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
	if (m_isDead)
	{
		return;
	}

	bool isTargetIn = m_enemy.lock().get()->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.lock().get()).Get();
	m_speed = m_enemy.lock().get()->GetTypeInfo().GetProperty("speed")->Get<float>(m_enemy.lock().get()).Get();
	m_passingTime += GetDeltaTime();

	if (!isTargetIn)
	{
		ComeBackHome();
		return;
	}
	if (m_canMove)
	{
		FollowTarget();
	}
}

void EnemyController::FollowTarget()
{
	if (!m_controller.expired() && !m_target.expired())
	{
		m_isComeBack = false;

		Vector3 pos = m_owner.lock()->GetWorldPosition();
		Vector3 targetPos = m_managers.lock()->Scene()->m_currentScene->FindPath(
			pos,
			m_target.lock()->GetWorldPosition(),
			GetScale()
		);

		Vector3 dir = targetPos - pos;
		float distance = sqrt(pow(dir.x, 2.f) + pow(dir.z, 2.f));
		float attackRange = m_enemy.lock().get()->GetTypeInfo().GetProperty("attackRange")->Get<float>(m_enemy.lock().get()).Get();

		// 일정거리까지 추적 후 멈춤
		if (distance > attackRange)
		{
			m_isBackStep = false;
			m_isAttackReady = false;
			dir.y = 0.0f;
			dir.Normalize(dir);
			dir.y = -100.0f;
			dir *= GetDeltaTime() * m_speed;
			m_controller.lock()->Move(dir);

		}
		// 뒷걸음질
		else if (distance < attackRange - 1.f)
		{
			m_isBackStep = true;
			m_isAttackReady = false;

			Vector3 backDir = dir;
			backDir.y = 0.0f;
			backDir.Normalize(backDir);
			backDir.y = -100.0f;
			backDir *= GetDeltaTime() * m_speed;

			backDir.x *= -1.f;
			backDir.z *= -1.f;
			m_controller.lock()->Move(backDir);
		}
		// 간보기
		else
		{
			m_isBackStep = false;
			m_isAttackReady = true;

			dir.y = 0.0f;
			dir.Normalize(dir);
			Vector3 right = -dir.Cross({ 0.f,1.f,0.f });


			if (m_passingTime > 3.f)
			{
				m_passingTime = 0.f;
				m_sideMove *= -1.f;
			}


			right *= m_sideMove;
			right *= GetDeltaTime() * m_speed;

			m_controller.lock()->Move(right);
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
		m_isAttackReady = false;

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
		// 돌아가기
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
		// 추적 해제
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
