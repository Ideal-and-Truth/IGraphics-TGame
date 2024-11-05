#include "EnemyController.h"
#include "Controller.h"
#include "Enemy.h"
#include "Transform.h"
#include "BossAnimator.h"
#include "PhysicsManager.h"

BOOST_CLASS_EXPORT_IMPLEMENT(EnemyController)

EnemyController::EnemyController()
	: m_speed(0.f)
	, m_isComeBack(false)
	, m_isAttackReady(false)
	, m_isBackStep(false)
	, m_isDead(false)
	, m_canMove(true)
	, m_strafeMove(false)
	, m_attackCharge(false)
	, m_useImpulse(false)
	, m_passingTime(0.f)
	, m_sideMove(1.f)
	, m_impulsePower(0.f)
	, m_sideImpulse(0.f)
	, m_isPassThrough(0.f)
	, m_delayTime(0.f)
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
	m_homePos = m_owner.lock()->GetWorldPosition();
	m_bossAnimator = m_owner.lock()->GetComponent<BossAnimator>().lock();
	m_controller.lock()->SetPosition(m_homePos);
	m_controller.lock()->SetGroup(static_cast<uint32>(Truth::COLLISION_GROUP::ENEMY));
	m_controller.lock()->SetMask(static_cast<uint32>(Truth::COLLISION_GROUP::ENEMY_MASK));
	m_controller.lock()->SetUpFiltering();
}

void EnemyController::FixedUpdate()
{
	if (!m_controller.expired())
	{
		if (!m_canMove)
		{
			m_moveVec = Vector3::Zero;
		}
		m_moveVec.y = -100.0f;
		m_controller.lock()->Move(m_moveVec);
	}
}

void EnemyController::Update()
{
	if (m_isDead)
		return;

	auto enemy = m_enemy.lock();

	bool isTargetIn = enemy->GetIsTargetIn();
	m_speed = enemy->GetSpeed();
	m_passingTime += GetDeltaTime();

	// 적 회전
	Vector3 pos = m_owner.lock()->GetWorldPosition();

	Vector3 playerPos = m_target.lock()->GetWorldPosition();

	Vector3 dir = m_targetPos - pos;

	float distance = (playerPos - pos).Length();
	float attackRange = enemy->GetAttackRange();

	dir.Normalize(dir);
	dir.y = 0.0f;

	Quaternion lookRot;
	Quaternion::LookRotation(dir, Vector3::Up, lookRot);

	if (distance > attackRange)
	{
		m_isBackStep = false;
		m_isAttackReady = false;
	}
	else
	{
		m_isBackStep = false;
		m_isAttackReady = true;
	}

	if (m_useImpulse)
	{
		Vector3 power(playerPos - pos);
		power.y = 0.f;
		power.Normalize();
		power.y = -100.f;
		power *= 0.002f;
		power.x *= m_impulsePower;
		power.y *= abs(m_impulsePower);
		power.z *= m_impulsePower;
		Vector3 p = power;
		if (m_sideImpulse > 0.f)
		{
			p.x = power.z;
			p.z = -power.x;
		}
		else if (m_sideImpulse < 0.f)
		{
			p.x = -power.z;
			p.z = power.x;
		}
		else if (m_sideImpulse == 0.f)
			p = power;

		m_controller.lock()->AddImpulse(p);
		if (m_isPassThrough)
			m_controller.lock()->SetUserData(true);

		m_useImpulse = false;
		m_impulsePower = 0.f;
		m_sideImpulse = 0.f;
	}

	if (!m_isPassThrough)
		m_controller.lock()->SetUserData(false);

	if (m_bossAnimator)
	{
		if (isTargetIn && !m_bossAnimator->GetIsLockOn())
		{
			Quaternion q = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_worldRotation, lookRot, 10.f * GetDeltaTime());
			SetWorldRotation(q);
		}
	}
	else
	{
		if (isTargetIn)
		{
			Quaternion q = Quaternion::Slerp(m_owner.lock().get()->m_transform->m_worldRotation, lookRot, 10.f * GetDeltaTime());
			SetWorldRotation(q);
		}
	}

	if (!isTargetIn)
	{
		ComeBackHome();
		m_moveVec.y = -100.0f;

		return;
	}

	if (m_canMove)
		FollowTarget();

	m_moveVec.y = -100.0f;
}

void EnemyController::FollowTarget()
{
	auto enemy = m_enemy.lock();

	m_delayTime += GetDeltaTime();
	if (!m_controller.expired() && !m_target.expired())
	{
		m_isComeBack = false;

		Vector3 pos = m_owner.lock()->GetWorldPosition();

		if (m_delayTime > 1.f || m_targetPos == Vector3::Zero)
		{
			m_targetPos = m_managers.lock()->Scene()->m_currentScene->FindPath(
				pos,
				m_target.lock()->GetWorldPosition(),
				GetScale()
			);

			m_delayTime = 0.f;
		}

		Vector3 playerPos = m_target.lock()->GetWorldPosition();

		Vector3 dir = m_targetPos - pos;

		float distance = (playerPos - pos).Length();
		float attackRange = enemy->GetAttackRange();

		// 일정거리까지 추적 후 멈춤
// 		if (distance > attackRange)
// 		{
// 			m_isBackStep = false;
// 			m_isAttackReady = false;
// 		}
// 		// 뒷걸음질
// //		else if (distance < attackRange - 1.f)
// //		{
// // 			m_isBackStep = true;
// // 			m_isAttackReady = false;
// //
// // 			Vector3 backDir = dir;
// // 			backDir.y = 0.0f;
// // 			backDir.Normalize(backDir);
// // 			backDir.y = -100.0f;
// // 			backDir *= GetDeltaTime() * m_speed;
// //
// // 			backDir.x *= -1.f;
// // 			backDir.z *= -1.f;
// // 			m_controller.lock()->Move(backDir);
// //		}
// 		// 간보기
// 		else
// 		{
// 			m_isBackStep = false;
// 			m_isAttackReady = true;
// 		}

		if (m_isAttackReady || m_strafeMove)
		{
			dir.y = 0.0f;
			dir.Normalize(dir);
			Vector3 right = -dir.Cross({ 0.f,1.f,0.f });

			if (m_passingTime > 3.f)
			{
				m_passingTime = 0.f;
				m_sideMove *= -1.f;
			}

			right *= m_sideMove;
			right *= m_speed;

			m_moveVec = right;
			//m_controller.lock()->Move(right);
		}
		else if (!m_isAttackReady && !m_strafeMove)
		{
			dir.y = 0.0f;
			dir.Normalize(dir);
			dir.y = -100.0f;
			dir *= m_speed * 2.f;

			m_moveVec = dir;
			//m_controller.lock()->Move(dir);
		}


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

		Vector3 homepos = m_managers.lock()->Scene()->m_currentScene->FindPath(
			pos,
			m_homePos,
			GetScale()
		);

		Vector3 dir = targetPos - pos;
		Vector3 backDir = homepos - pos;

		Quaternion lookRot;
		Quaternion rot = m_owner.lock()->m_transform->m_rotation;

		Vector2 disPos = { pos.x, pos.z };
		Vector2 disHomePos = { m_homePos.x, m_homePos.z };
		float distance = (disPos - disHomePos).Length();
		// 돌아가기
		if (distance > 1.0f)
		{
			m_isComeBack = true;
			backDir.y = 0.0f;
			backDir.Normalize(backDir);
			backDir.y = -100.0f;
			backDir *= m_speed * 0.5f;
			m_controller.lock()->Move(backDir);
			m_moveVec = backDir;
			//m_controller.lock()->Move(backDir);
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
