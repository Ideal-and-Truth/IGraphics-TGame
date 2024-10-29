#include "EnemyAnimator.h"
#include "SkinnedMesh.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "PlayerAnimator.h"
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
	, m_isChargeAttack(false)
	, m_isBack(false)
	, m_isFall(false)
	, m_isDown(false)
	, m_isDamage(false)
	, m_isDeath(false)
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

	m_animationStateMap["ChargeAttack"] = new EnemyChargeAttack(this);

	m_animationStateMap["Hit"] = new EnemyHit(this);

	m_animationStateMap["Down"] = new EnemyDown(this);

	m_animationStateMap["Fall"] = new EnemyFall(this);

	m_animationStateMap["KnockBack"] = new EnemyKnockBack(this);

	m_animationStateMap["Death"] = new EnemyDeath(this);

	m_currentState = m_animationStateMap["Idle"];
}

void EnemyAnimator::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_enemyController = m_owner.lock().get()->GetComponent<EnemyController>().lock();
	m_enemy = m_owner.lock().get()->GetComponent<Enemy>().lock();
	auto playerEntity = m_enemy->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.get()).Get().lock();
	m_playerAnimator = playerEntity->GetComponent<PlayerAnimator>().lock();

	m_skinnedMesh->AddAnimation("EnemyMeleeAttack", L"EnemyAnimations/MeleeEnemy/Attack/Attack");

	m_skinnedMesh->AddAnimation("EnemyDown", L"EnemyAnimations/MeleeEnemy/Down/Down");
	m_skinnedMesh->AddAnimation("EnemyFall", L"EnemyAnimations/MeleeEnemy/Down/Fall");
	m_skinnedMesh->AddAnimation("EnemyFallAttack", L"EnemyAnimations/MeleeEnemy/Down/FallAttack");
	m_skinnedMesh->AddAnimation("EnemyGetUp", L"EnemyAnimations/MeleeEnemy/Down/GetUp");
	m_skinnedMesh->AddAnimation("EnemyKnockBack", L"EnemyAnimations/MeleeEnemy/Down/KnockBack");

	m_skinnedMesh->AddAnimation("EnemyMeleeHit", L"EnemyAnimations/MeleeEnemy/Hit/Hit");
	m_skinnedMesh->AddAnimation("EnemyMeleeIdle", L"EnemyAnimations/MeleeEnemy/Idle/Idle");
	m_skinnedMesh->AddAnimation("EnemyMeleePursuit", L"EnemyAnimations/MeleeEnemy/Pursuit/Pursuit");
	m_skinnedMesh->AddAnimation("EnemyMeleeReturn", L"EnemyAnimations/MeleeEnemy/Return/Return");
	m_skinnedMesh->AddAnimation("EnemyMeleeStrafeL", L"EnemyAnimations/MeleeEnemy/StrafeMove/StrafeL");
	m_skinnedMesh->AddAnimation("EnemyMeleeStrafeR", L"EnemyAnimations/MeleeEnemy/StrafeMove/StrafeR");
	m_skinnedMesh->AddAnimation("EnemyMeleeStrongAttack", L"EnemyAnimations/MeleeEnemy/StrongAttack/StrongAttack");


	// 과거의 잔재
// 	m_skinnedMesh->AddAnimation("EnemyIdle", L"Kachujin/Idle");
// 	m_skinnedMesh->AddAnimation("EnemyWalk", L"Kachujin/Sword And Shield Walk");
// 	m_skinnedMesh->AddAnimation("EnemyLeftWalk", L"Kachujin/Left Strafe Walking");
// 	m_skinnedMesh->AddAnimation("EnemyRightWalk", L"Kachujin/Right Strafe Walking");
// 	m_skinnedMesh->AddAnimation("EnemyBackStep", L"Kachujin/Standing Walk Back");
// 	m_skinnedMesh->AddAnimation("EnemyAttack", L"Kachujin/Sword And Shield Slash");
// 	m_skinnedMesh->AddAnimation("EnemyParriableAttack", L"Kachujin/Jump Attack");
// 	m_skinnedMesh->AddAnimation("EnemyHit", L"Kachujin/Sword And Shield Impact");
// 	m_skinnedMesh->AddAnimation("EnemyDeath", L"Kachujin/Sword And Shield Death");
// 	m_skinnedMesh->AddAnimation("EnemyParried", L"Kachujin/Parried");

	m_currentState->OnStateEnter();
}

void EnemyAnimator::Update()
{
	if (m_isDeath)
	{
		m_owner.lock()->Destroy();
		return;
	}

	if (m_isDeath)
	{
		return;
	}

	m_currentFrame = m_skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(m_skinnedMesh.get()).Get();
	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();
	m_isChase = m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.get()).Get();
	m_isComeBack = m_enemyController->GetTypeInfo().GetProperty("isComeBack")->Get<bool>(m_enemyController.get()).Get();
	m_isAttackReady = m_enemyController->GetTypeInfo().GetProperty("isAttackReady")->Get<bool>(m_enemyController.get()).Get();
	m_isBackStep = m_enemyController->GetTypeInfo().GetProperty("isBackStep")->Get<bool>(m_enemyController.get()).Get();
	m_sideMove = m_enemyController->GetTypeInfo().GetProperty("sideMove")->Get<float>(m_enemyController.get()).Get();

	if (m_enemy->GetTypeInfo().GetProperty("slowTime")->Get<bool>(m_enemy.get()).Get())
	{
		m_skinnedMesh->SetAnimationSpeed(0.3f);
	}
	else
	{
		m_skinnedMesh->SetAnimationSpeed(1.f);
	}

	if (m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() <= 0.f)
	{
		m_isDeath = true;
		m_enemyController->GetTypeInfo().GetProperty("isDead")->Set(m_enemyController.get(), true);
	}

	if (m_lastHp > m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() && m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() > 0.f)
	{
		m_isDamage = true;
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
		
		if (m_playerAnimator->GetTypeInfo().GetProperty("backAttack")->Get<bool>(m_playerAnimator.get()).Get())
		{
			m_isBack = true;
			m_isDamage = false;
		}
		else if (m_playerAnimator->GetTypeInfo().GetProperty("fallAttack")->Get<bool>(m_playerAnimator.get()).Get())
		{
			m_isFall = true;
			m_isDamage = false;
		}
		else if (m_playerAnimator->GetTypeInfo().GetProperty("downAttack")->Get<bool>(m_playerAnimator.get()).Get())
		{
			m_isDown = true;
			m_isDamage = false;
		}
	}

	if (m_isAttackReady && !m_isAttack && !m_isChargeAttack && !m_isDown)
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 2.f)
		{
			int random = RandomNumber(1, 5);
			if (random >= 1 && random <= 2)
			{
				m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
				m_isChargeAttack = true;
			}
			else if (random >= 3 && random <= 5)
			{
				m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
				m_isAttack = true;
			}
			m_isAttacking = true;
		}
	}

	if (m_isAttack || m_currentState == m_animationStateMap["Attack"] || m_currentState == m_animationStateMap["ChargeAttack"]
		|| m_currentState == m_animationStateMap["Hit"] || m_currentState == m_animationStateMap["Down"] || m_currentState == m_animationStateMap["Fall"] || m_currentState == m_animationStateMap["KnockBack"])
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
	}
	else
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
	}

	if (m_isDown || m_isFall  || m_isBack)
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
	}

	if (!m_isAttack && !m_isDamage && !m_isChargeAttack && !m_isDown && !m_isFall &&  !m_isBack)
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

void EnemyAnimator::SetImpulse(float power)
{
	m_enemyController->GetTypeInfo().GetProperty("useImpulse")->Set(m_enemyController.get(), true);
	m_enemyController->GetTypeInfo().GetProperty("impulsePower")->Set(m_enemyController.get(), power);
}

void EnemyIdle::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleeIdle", false);
}

void EnemyIdle::OnStateUpdate()
{
	if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Death");
	}
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Hit");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("KnockBack");
	}
	else if (GetProperty("isChase")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Chase");
	}
}

void EnemyChase::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleePursuit", false);
}

void EnemyChase::OnStateUpdate()
{
	if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Death");
	}
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Hit");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("KnockBack");
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
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleeReturn", false);
}

void EnemyReturn::OnStateUpdate()
{
	if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Death");
	}
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
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
			dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleeStrafeL", false);
		}
		else
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleeStrafeR", false);
		}
	}

	if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Death");
	}
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Hit");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("KnockBack");
	}
	else if (!GetProperty("isChase")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Return");
	}
	else if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Attack");
	}
	else if (GetProperty("isChargeAttack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("ChargeAttack");
	}
	else if (!GetProperty("isAttackReady")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Chase");
	}
}

void EnemyAttack::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleeAttack", false);
	dynamic_cast<EnemyAnimator*>(m_animator)->SetImpulse(50.f);
}

void EnemyAttack::OnStateUpdate()
{
	if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Hit");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("KnockBack");
	}

	if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
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

void EnemyChargeAttack::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleeStrongAttack", false);
	dynamic_cast<EnemyAnimator*>(m_animator)->SetImpulse(150.f);
}

void EnemyChargeAttack::OnStateUpdate()
{
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		GetProperty("isChargeAttack")->Set(m_animator, false);
		GetProperty("passingTime")->Set(m_animator, 0.f);
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Down");
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		GetProperty("passingTime")->Set(m_animator, 0.f);
		GetProperty("isChargeAttack")->Set(m_animator, false);
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("AttackReady");
	}
}


void EnemyChargeAttack::OnStateExit()
{
	GetProperty("isAttacking")->Set(m_animator, false);
}

void EnemyHit::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleeHit", false);
	GetProperty("isDamage")->Set(m_animator, false);
}

void EnemyHit::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("AttackReady");
	}
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleeHit", false);
		GetProperty("isDamage")->Set(m_animator, false);
	}
}

void EnemyDown::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyDown", false);
	GetProperty("isDown")->Set(m_animator, false);
}

void EnemyDown::OnStateUpdate()
{
	if (isReset && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Idle");
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		isReset = true;
		dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyGetUp", false);
	}
}

void EnemyDown::OnStateExit()
{
	isReset = false;
}

void EnemyFall::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyFall", false);
	GetProperty("isFall")->Set(m_animator, false);
}

void EnemyFall::OnStateUpdate()
{
	if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		GetProperty("isFall")->Set(m_animator, false);
		dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyFallAttack", false);
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Down");
	}
}

void EnemyFall::OnStateExit()
{
	GetProperty("isFall")->Set(m_animator, false);
}

void EnemyKnockBack::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyKnockBack", false);
	GetProperty("isBack")->Set(m_animator, false);
}

void EnemyKnockBack::OnStateUpdate()
{
	if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		GetProperty("isBack")->Set(m_animator, false);
		dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyKnockBack", false);
	}
	if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Fall");
	}
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Down");
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void EnemyKnockBack::OnStateExit()
{

}

void EnemyDeath::OnStateEnter()
{
	//dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyDeath", false);
}

void EnemyDeath::OnStateUpdate()
{
	// 	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 116)
	// 	{
	// 		dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimationPlay(false);
	// 	}
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

