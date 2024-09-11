#include "EnemyAnimator.h"
#include "SkinnedMesh.h"
#include "Enemy.h"
#include "EnemyController.h"
#include <random>

BOOST_CLASS_EXPORT_IMPLEMENT(EnemyAnimator)


EnemyAnimator::EnemyAnimator()
	: m_skinnedMesh(nullptr)
	, m_enemy(nullptr)
	, m_enemyController(nullptr)
	, m_currentState(nullptr)
	, m_currentFrame(0)
	, m_hitStopTime(0.f)
	, m_passingTime(0.f)
	, m_sideMove(0.f)
	, m_lastHp(0.f)
	, m_isChase(false)
	, m_isAttackReady(false)
	, m_isAttack(false)
	, m_isParryAttack(false)
	, m_isParried(false)
	, m_isHit(false)
	, m_isDead(false)
	, m_isComeBack(false)
	, m_isAnimationEnd(false)
	, m_isBackStep(false)
	, m_isAttacking(false)
{

}

EnemyAnimator::~EnemyAnimator()
{

}

void EnemyAnimator::Awake()
{
	m_animationStateMap["Idle"] = new EnemyIdle(this);
	m_animationStateMap["Chase"] = new EnemyChase(this);
	m_animationStateMap["Return"] = new EnemyReturn(this);
	m_animationStateMap["AttackReady"] = new EnemyAttackReady(this);
	m_animationStateMap["Attack"] = new EnemyAttack(this);
	m_animationStateMap["ParriableAttack"] = new EnemyParriableAttack(this);
	m_animationStateMap["Hit"] = new EnemyHit(this);
	m_animationStateMap["Parried"] = new EnemyParried(this);
	m_animationStateMap["Death"] = new EnemyDeath(this);

	m_currentState = m_animationStateMap["Idle"];
}

void EnemyAnimator::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_enemyController = m_owner.lock().get()->GetComponent<EnemyController>().lock();
	m_enemy = m_owner.lock().get()->GetComponent<Enemy>().lock();

	m_skinnedMesh->AddAnimation("EnemyIdle", L"AsciiAniTest/idelTest");
	m_skinnedMesh->AddAnimation("EnemyWalk", L"Kachujin/Sword And Shield Walk");
	m_skinnedMesh->AddAnimation("EnemyLeftWalk", L"Kachujin/Left Strafe Walking");
	m_skinnedMesh->AddAnimation("EnemyRightWalk", L"Kachujin/Right Strafe Walking");
	m_skinnedMesh->AddAnimation("EnemyBackStep", L"Kachujin/Standing Walk Back");
	m_skinnedMesh->AddAnimation("EnemyAttack", L"Kachujin/Sword And Shield Slash");
	m_skinnedMesh->AddAnimation("EnemyParriableAttack", L"Kachujin/Jump Attack");
	m_skinnedMesh->AddAnimation("EnemyHit", L"Kachujin/Sword And Shield Impact");
	m_skinnedMesh->AddAnimation("EnemyDeath", L"Kachujin/Sword And Shield Death");
	m_skinnedMesh->AddAnimation("EnemyParried", L"Kachujin/Parried");

	m_currentState->OnStateEnter();
}

void EnemyAnimator::Update()
{
	m_currentFrame = m_skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(m_skinnedMesh.get()).Get();
	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();
	m_isChase = m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.get()).Get();
	m_isComeBack = m_enemyController->GetTypeInfo().GetProperty("isComeBack")->Get<bool>(m_enemyController.get()).Get();
	m_isAttackReady = m_enemyController->GetTypeInfo().GetProperty("isAttackReady")->Get<bool>(m_enemyController.get()).Get();
	m_isBackStep = m_enemyController->GetTypeInfo().GetProperty("isBackStep")->Get<bool>(m_enemyController.get()).Get();
	m_sideMove = m_enemyController->GetTypeInfo().GetProperty("sideMove")->Get<float>(m_enemyController.get()).Get();

	if (m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() <= 0.f)
	{
		m_isDead = true;
		m_enemyController->GetTypeInfo().GetProperty("isDead")->Set(m_enemyController.get(), true);
	}

	if (m_lastHp > m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() && m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() > 0.f)
	{
		m_isHit = true;
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
	}

	if (m_isAttackReady && !m_isAttack && !m_isParryAttack && !m_isParried)
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 2.f)
		{
			int random = RandomNumber(1, 5);
			if (random >= 1 && random <= 2)
			{
				//m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
				//m_isParryAttack = true;
			}
			else if (random >= 1 && random <= 5)
			{
				m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
				m_isAttack = true;
			}
			m_isAttacking = true;
		}
	}

	if (m_isParried)
	{
		m_passingTime += GetDeltaTime();
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
	}

	if (!m_isAttack && !m_isHit && !m_isParryAttack && !m_isParried)
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
	}
	m_currentState->OnStateUpdate();
	m_lastHp = m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get();
}

void EnemyAnimator::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
}

void EnemyAnimator::SetAnimationSpeed(float speed)
{
	m_skinnedMesh->SetAnimationSpeed(speed);
}

void EnemyAnimator::SetAnimationPlay(bool playStop)
{
	m_skinnedMesh->SetPlayStop(playStop);
}

void EnemyAnimator::ChangeState(std::string stateName)
{
	m_currentState->OnStateExit();
	m_currentState = m_animationStateMap[stateName];
	m_currentState->OnStateEnter();
}

void EnemyAnimator::SetEnemyDamage(float damage)
{
	m_enemy->GetTypeInfo().GetProperty("currentDamage")->Set(m_enemy.get(), damage);
}

void EnemyIdle::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyIdle", false);
}

void EnemyIdle::OnStateUpdate()
{
	if (GetProperty("isDead")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Death");
	}
	else if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Hit");
	}
	else if (GetProperty("isChase")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Chase");
	}
}

void EnemyChase::OnStateEnter()
{
	m_isChangePose = true;
}

void EnemyChase::OnStateUpdate()
{
	if (m_isBackStep != GetProperty("isBackStep")->Get<bool>(m_animator).Get())
	{
		m_isChangePose = true;
		m_isBackStep = GetProperty("isBackStep")->Get<bool>(m_animator).Get();
	}
	if (m_isChangePose)
	{
		m_isChangePose = false;
		if (!m_isBackStep)
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyWalk", false);
		}
		else
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyBackStep", false);
		}
	}

	if (GetProperty("isDead")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Death");
	}
	else if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Hit");
	}
	else if (!GetProperty("isChase")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Return");
	}
	else if (GetProperty("isAttackReady")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("AttackReady");
	}

}

void EnemyReturn::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyWalk", false);
}

void EnemyReturn::OnStateUpdate()
{
	if (GetProperty("isDead")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Death");
	}
	else if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Hit");
	}
	else if (!GetProperty("isComeBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void EnemyAttackReady::OnStateEnter()
{
	m_isChangePose = true;
}

void EnemyAttackReady::OnStateUpdate()
{
	if (m_sidePose != GetProperty("sideMove")->Get<float>(m_animator).Get())
	{
		m_isChangePose = true;
		m_sidePose = GetProperty("sideMove")->Get<float>(m_animator).Get();
	}
	if (m_isChangePose)
	{
		m_isChangePose = false;
		if (m_sidePose > 0.f)
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyRightWalk", false);
		}
		else
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyLeftWalk", false);
		}
	}

	if (GetProperty("isDead")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Death");
	}
	else if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Hit");
	}
	else if (!GetProperty("isChase")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Return");
	}
	else if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Attack");
	}
	else if (GetProperty("isParryAttack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("ParriableAttack");
	}
	else if (!GetProperty("isAttackReady")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Chase");
	}
}

void EnemyAttack::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyAttack", false);
}

void EnemyAttack::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		GetProperty("isHit")->Set(m_animator, false);
	}

	if (GetProperty("isDead")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Death");
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		GetProperty("isAttack")->Set(m_animator, false);
		GetProperty("passingTime")->Set(m_animator, 0.f);

		if (GetProperty("isAttackReady")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("AttackReady");
		}
		else if (GetProperty("isChase")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Chase");
		}
		else if (!GetProperty("isChase")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Return");
		}
	}
}

void EnemyAttack::OnStateExit()
{
	GetProperty("isAttacking")->Set(m_animator, false);
}

void EnemyParriableAttack::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyParriableAttack", false);
}

void EnemyParriableAttack::OnStateUpdate()
{
	if (GetProperty("isParried")->Get<bool>(m_animator).Get())
	{
		GetProperty("isParryAttack")->Set(m_animator, false);
		GetProperty("passingTime")->Set(m_animator, 0.f);
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Parried");
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		GetProperty("passingTime")->Set(m_animator, 0.f);
		GetProperty("isParryAttack")->Set(m_animator, false);
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("AttackReady");
	}
}


void EnemyParriableAttack::OnStateExit()
{
	GetProperty("isAttacking")->Set(m_animator, false);
}

void EnemyHit::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyHit", false);
}

void EnemyHit::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		GetProperty("isHit")->Set(m_animator, false);
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("AttackReady");
	}
}

void EnemyParried::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyParried", false);
}

void EnemyParried::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		GetProperty("passingTime")->Set(m_animator, 0.f);
		GetProperty("isParried")->Set(m_animator, false);
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Hit");
	}
	if (GetProperty("passingTime")->Get<float>(m_animator).Get() > 3.f)
	{
		GetProperty("isParried")->Set(m_animator, false);
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("AttackReady");
	}
}

void EnemyDeath::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyDeath", false);
}

void EnemyDeath::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 116)
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimationPlay(false);
	}
}


int EnemyAnimator::RandomNumber(int _min, int _max)
{
	std::random_device rd;
	// 시드값으로 난수 생성 엔진 초기화
	std::mt19937 gen(rd());
	// 균등 분포 정의
	std::uniform_int_distribution<int> dis(_min, _max);
	// 난수 반환
	return dis(gen);
}