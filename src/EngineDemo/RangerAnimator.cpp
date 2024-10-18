#include "RangerAnimator.h"
#include "SkinnedMesh.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "PlayerAnimator.h"
#include "Transform.h"
#include <random>

BOOST_CLASS_EXPORT_IMPLEMENT(RangerAnimator)

int RangerAnimator::RandomNumber(int _min, int _max)
{
	std::random_device rd;
	// 시드값으로 난수 생성 엔진 초기화
	std::mt19937 gen(rd());
	// 균등 분포 정의
	std::uniform_int_distribution<int> dis(_min, _max);
	// 난수 반환
	return dis(gen);
}

RangerAnimator::RangerAnimator()
	: m_isPursuit(false)
	, m_isReturn(false)
	, m_isAttack(false)
	, m_isDamage(false)
	, m_isDown(false)
	, m_isDeath(false)
	, m_isReload(false)
	, m_isShooting(false)
	, m_isAnimationEnd(false)
	, m_passingTime(0.f)
	, m_lastHp(0.f)
	, m_hitStopTime(0.f)
	, m_currentFrame(0)
	, m_currentState(nullptr)
{

}

RangerAnimator::~RangerAnimator()
{

}

void RangerAnimator::Awake()
{
	m_animationStateMap["Idle"] = new RangerIdle(this);

	m_animationStateMap["Pursuit"] = new RangerPursuit(this);

	m_animationStateMap["Return"] = new RangerReturn(this);

	m_animationStateMap["Attack"] = new RangerAttack(this);

	m_animationStateMap["Aim"] = new RangerAim(this);

	m_animationStateMap["Reload"] = new RangerReload(this);

	m_animationStateMap["Damage"] = new RangerDamage(this);

	m_animationStateMap["Down"] = new RangerDown(this);

	m_animationStateMap["Death"] = new RangerDeath(this);


	m_currentState = m_animationStateMap["Idle"];
}

void RangerAnimator::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_enemyController = m_owner.lock().get()->GetComponent<EnemyController>().lock();
	m_enemy = m_owner.lock().get()->GetComponent<Enemy>().lock();
	auto playerEntity = m_enemy->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.get()).Get().lock();
	m_playerAnimator = playerEntity->GetComponent<PlayerAnimator>().lock();

	m_skinnedMesh->AddAnimation("EnemyRangeAim", L"EnemyAnimations/RangeEnemy/Aim/Aim");
	m_skinnedMesh->AddAnimation("EnemyRangeAttack", L"EnemyAnimations/RangeEnemy/Attack/Attack");
	m_skinnedMesh->AddAnimation("EnemyRangeDown", L"EnemyAnimations/RangeEnemy/Down/Down");
	m_skinnedMesh->AddAnimation("EnemyRangeHit", L"EnemyAnimations/RangeEnemy/Hit/Hit");
	m_skinnedMesh->AddAnimation("EnemyRangeIdle", L"EnemyAnimations/RangeEnemy/Idle/Idle");
	m_skinnedMesh->AddAnimation("EnemyRangePursuitReturn", L"EnemyAnimations/RangeEnemy/Pursuit Return/PursuitReturn");
	m_skinnedMesh->AddAnimation("EnemyRangeReload", L"EnemyAnimations/RangeEnemy/Reload/Reload");

	m_currentState->OnStateEnter();
}

void RangerAnimator::Update()
{
	if (m_isDeath)
	{
		for (auto& e : m_owner.lock()->m_children)
		{ 
			m_owner.lock()->DeleteChild(e);
			m_owner.lock().reset();
			m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e);
		}
		/// 런타임 중 리지드바디 삭제시 오류
		m_managers.lock()->Scene()->m_currentScene->DeleteEntity(m_owner.lock());
		// m_owner.lock()->m_transform->m_scale = { 0.f,0.f,0.f };
		return;
	}

	if (m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() <= 0.f)
	{
		m_isDeath = true;
	}

	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();
	m_currentFrame = m_skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(m_skinnedMesh.get()).Get();
	m_isPursuit = m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.get()).Get();
	m_isAttack = m_enemyController->GetTypeInfo().GetProperty("isAttackReady")->Get<bool>(m_enemyController.get()).Get();

	if (m_isReload)
	{
		m_passingTime += GetDeltaTime();
	}

	if (m_isAttack)
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
	}
	else
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
	}

	if (m_isReload)
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
	}


	if (m_lastHp > m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get())
	{
		m_isDamage = true;
		if (m_playerAnimator->GetTypeInfo().GetProperty("downAttack")->Get<bool>(m_playerAnimator.get()).Get())
		{
			m_isDown = true;
			m_isDamage = false;
		}
	}



	m_lastHp = m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get();

	m_currentState->OnStateUpdate();
}

void RangerAnimator::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
}

void RangerAnimator::SetAnimationSpeed(float speed)
{
	m_skinnedMesh->SetAnimationSpeed(speed);
}

void RangerAnimator::SetAnimationPlay(bool playStop)
{
	m_skinnedMesh->SetPlayStop(playStop);
}

void RangerAnimator::ChangeState(std::string stateName)
{
	m_currentState->OnStateExit();
	m_currentState = m_animationStateMap[stateName];
	m_currentState->OnStateEnter();
}

void RangerAnimator::SetEnemyDamage(float damage)
{
	m_enemy->GetTypeInfo().GetProperty("currentDamage")->Set(m_enemy.get(), damage);
}

void RangerIdle::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangeIdle", false);
}

void RangerIdle::OnStateUpdate()
{
	if (GetProperty("isPursuit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Pursuit");
	}
	else if (GetProperty("isReturn")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Return");
	}

	// Any State
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Damage");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}
}

void RangerPursuit::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangePursuitReturn", false);
}

void RangerPursuit::OnStateUpdate()
{
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Aim");
	}

	// Any State
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Damage");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}
}

void RangerReturn::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangePursuitReturn", false);
}

void RangerReturn::OnStateUpdate()
{
	if (GetProperty("isPursuit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Pursuit");
	}
	else if (!GetProperty("isReturn")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Idle");
	}

	// Any State
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Damage");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}
}

void RangerAttack::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangeAttack", false);
}

void RangerAttack::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 9)
	{
		GetProperty("isShooting")->Set(m_animator, true);
	}

	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Reload");
		}
		else if (!GetProperty("isAttack")->Get<bool>(m_animator).Get() && GetProperty("isPursuit")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Pursuit");
		}
		else if (!GetProperty("isAttack")->Get<bool>(m_animator).Get() && !GetProperty("isPursuit")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}

	// Any State
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Damage");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}
}

void RangerAttack::OnStateExit()
{
	GetProperty("isShooting")->Set(m_animator, false);
}

void RangerAim::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangeAim", false);
}

void RangerAim::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Attack");
		}
		else if (!GetProperty("isAttack")->Get<bool>(m_animator).Get() && GetProperty("isPursuit")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Pursuit");
		}
		else if (!GetProperty("isAttack")->Get<bool>(m_animator).Get() && !GetProperty("isPursuit")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}

	// Any State
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Damage");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}
}

void RangerReload::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangeReload", false);
	GetProperty("isReload")->Set(m_animator, true);
}

void RangerReload::OnStateUpdate()
{
	if (GetProperty("passingTime")->Get<float>(m_animator).Get() > 1.f)
	{
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Aim");
		}
		else if (!GetProperty("isAttack")->Get<bool>(m_animator).Get() && !GetProperty("isPursuit")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Idle");
		}
		GetProperty("passingTime")->Set(m_animator, 0.f);
	}

	// Any State
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Damage");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}
}

void RangerReload::OnStateExit()
{
	GetProperty("isReload")->Set(m_animator, false);
}

void RangerDamage::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangeHit", false);
	GetProperty("isDamage")->Set(m_animator, false);
}

void RangerDamage::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Aim");
		}
		else
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangeHit", false);
		GetProperty("isDamage")->Set(m_animator, false);
	}

	// Any State
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}
}

void RangerDamage::OnStateExit()
{
	GetProperty("isDamage")->Set(m_animator, false);
}

void RangerDown::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangeDown", false);
}

void RangerDown::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Aim");
		}
		else
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}

	// Any State
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}
}

void RangerDown::OnStateExit()
{
	GetProperty("isDown")->Set(m_animator, false);
}

void RangerDeath::OnStateEnter()
{

}

void RangerDeath::OnStateUpdate()
{

}
