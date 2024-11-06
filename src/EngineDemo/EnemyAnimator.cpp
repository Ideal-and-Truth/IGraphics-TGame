#include "EnemyAnimator.h"
#include "SkinnedMesh.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "PlayerAnimator.h"
#include <random>
#include "ParticleManager.h"
#include "IParticleSystem.h"
#include "SoundManager.h"

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
	, m_baseSpeed(0.f)
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
	, m_normalAttack(false)
	, m_chargeAttack(false)
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
	m_baseSpeed = m_enemy->GetTypeInfo().GetProperty("speed")->Get<float>(m_enemy.get()).Get();

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

	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Enemy\\Enemy_Walk_1_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_1_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_2_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_3_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_4_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_5_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_6_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Dead_1_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Dead_2_Sound.wav", false);

	m_currentState->OnStateEnter();
}

void EnemyAnimator::Update()
{
	if (m_isDeath)
	{
		return;
	}

	m_currentFrame = m_skinnedMesh->GetCurrentFrame();
	m_isAnimationEnd = m_skinnedMesh->GetIsAnimationEnd();
	m_isChase = m_enemy->GetIsTargetIn();
	m_isAttackReady = m_enemyController->GetIsAttackReady();
	m_sideMove = m_enemyController->GetSideMove();
	// m_isComeBack = m_enemyController->GetTypeInfo().GetProperty("isComeBack")->Get<bool>(m_enemyController.get()).Get();
	// m_isBackStep = m_enemyController->GetIsBackStep();

	if (m_enemy->GetSlowTime())
	{
		m_skinnedMesh->SetAnimationSpeed(0.3f);
	}
	else
	{
		m_skinnedMesh->SetAnimationSpeed(1.f);
	}

	if (m_enemy->GetCurrentTP() <= 0.f)
	{
		int random = RandomNumber(1, 2);

		if (random == 1)
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Dead_1_Sound.wav", false, 39);
		else if (random == 2)
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Dead_2_Sound.wav", false, 39);

		m_isDeath = true;
		m_enemyController->SetIsDead(true);
	}

	if (m_lastHp > m_enemy->GetCurrentTP() && m_enemy->GetCurrentTP() > 0.f)
	{
		m_isDamage = true;
		m_enemyController->SetCanMove(false);


		if (m_playerAnimator->GetBackAttack())
		{
			int random = RandomNumber(1, 4);

			if (random == 1)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_1_Sound.wav", false, 40);
			else if (random == 2)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_2_Sound.wav", false, 40);
			else if (random == 3)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_3_Sound.wav", false, 40);
			else if (random == 4)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_4_Sound.wav", false, 40);

			m_isBack = true;
			m_isDamage = false;
		}
		else if (m_playerAnimator->GetFallAttack())
		{
			int random = RandomNumber(1, 2);

			if (random == 1)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_5_Sound.wav", false, 41);
			else if (random == 2)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_6_Sound.wav", false, 41);


			m_isFall = true;
			m_isDamage = false;
		}
		else if (m_playerAnimator->GetDownAttack())
		{
			int random = RandomNumber(1, 2);

			if (random == 1)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_5_Sound.wav", false, 42);
			else if (random == 2)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_6_Sound.wav", false, 42);

			m_isDown = true;
			m_isDamage = false;
		}
		else
		{
			int random = RandomNumber(1, 4);

			if (random == 1)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_1_Sound.wav", false, 43);
			else if (random == 2)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_2_Sound.wav", false, 43);
			else if (random == 3)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_3_Sound.wav", false, 43);
			else if (random == 4)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_4_Sound.wav", false, 43);
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
				m_enemyController->SetCanMove(false);
				m_isChargeAttack = true;
			}
			else if (random >= 3 && random <= 5)
			{
				m_enemyController->SetCanMove(false);
				m_isAttack = true;
			}
		}
	}

	// TODO: vector로 최적화
	if (m_isAttack || m_currentState == m_animationStateMap["Attack"] || m_currentState == m_animationStateMap["ChargeAttack"]
		|| m_currentState == m_animationStateMap["Hit"] || m_currentState == m_animationStateMap["Down"] || m_currentState == m_animationStateMap["Fall"] || m_currentState == m_animationStateMap["KnockBack"])
		m_enemyController->SetCanMove(false);
	else
		m_enemyController->SetCanMove(true);


	if (m_isChase)
	{
		m_enemy->GetTypeInfo().GetProperty("speed")->Set(m_enemy.get(), m_baseSpeed * 1.5f);
	}
	if (m_isAttackReady)
	{
		m_enemy->GetTypeInfo().GetProperty("speed")->Set(m_enemy.get(), m_baseSpeed);
	}

	m_currentState->OnStateUpdate();
	PlayEffect();

	m_lastHp = m_enemy->GetCurrentTP();
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
	lastFrame = 0;
}

void EnemyChase::OnStateUpdate()
{
	if ((GetProperty("currentFrame")->Get<int>(m_animator).Get() == 6 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 16)
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Enemy\\Enemy_Walk_1_Sound.wav", true, 44);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
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
	lastFrame = 0;
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

	if (m_sidePose > 0.f)
	{
		if ((GetProperty("currentFrame")->Get<int>(m_animator).Get() == 16 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 33)
			&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Enemy\\Enemy_Walk_1_Sound.wav", true, 44);
			lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
		}
	}
	else
	{
		if ((GetProperty("currentFrame")->Get<int>(m_animator).Get() == 18 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 36)
			&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
		{
			dynamic_cast<EnemyAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Enemy\\Enemy_Walk_1_Sound.wav", true, 44);
			lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
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
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 16)
	{
		GetProperty("normalAttack")->Set(m_animator, true);
		GetProperty("isAttacking")->Set(m_animator, true);
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 26)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}

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
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isAttack")->Set(m_animator, false);
}

void EnemyChargeAttack::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyMeleeStrongAttack", false);
	dynamic_cast<EnemyAnimator*>(m_animator)->SetImpulse(150.f);
}

void EnemyChargeAttack::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 19)
	{
		GetProperty("chargeAttack")->Set(m_animator, true);
		GetProperty("isAttacking")->Set(m_animator, true);
	}

	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 26)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}

	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		GetProperty("isChargeAttack")->Set(m_animator, false);
		GetProperty("passingTime")->Set(m_animator, 0.f);
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		GetProperty("isChargeAttack")->Set(m_animator, false);
		GetProperty("passingTime")->Set(m_animator, 0.f);
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("KnockBack");
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
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isAttack")->Set(m_animator, false);
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
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isChange = true;
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		isReset = true;
		dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyGetUp", false);
	}
	if (isChange && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Idle");
	}

}

void EnemyDown::OnStateExit()
{
	GetProperty("isDown")->Set(m_animator, false);
	isReset = false;
	isChange = false;
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
	dynamic_cast<EnemyAnimator*>(m_animator)->SetImpulse(-20.f);
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

void EnemyAnimator::PlayEffect()
{
	Vector3 effectPos = m_owner.lock()->GetWorldPosition();
	Vector3 effectRot = m_owner.lock()->GetWorldRotation().ToEuler();

	if (m_normalAttack)
	{
		m_normalAttack = false;

		m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\02 Combat_Sound\\Player_Swing_1_Sound.wav", true, 37);
		// 		effectPos.y += 1.f;
		// 
		// 		effectRot.y += (3.141592f / 180.f) * -140.f;
		// 		effectRot.z += (3.141592f / 180.f) * 180.f;
		// 
		// 		Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));
		// 
		// 		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\EnemySlash.yaml");
		// 		p->SetTransformMatrix(
		// 			rotationMT
		// 			* Matrix::CreateTranslation(effectPos)
		// 		);
		// 
		// 		p->SetSimulationSpeed(2.f);
		// 		p->SetActive(true);
		// 		p->Play();
	}

	if (m_chargeAttack)
	{
		m_chargeAttack = false;

		{
			effectPos.y += 0.5f;

			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\EnemyCharge0.yaml");
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(0.5, 0.5, 1))
				* rotationMT
				* Matrix::CreateTranslation(effectPos)
			);

			p->SetSimulationSpeed(2.5f);
			p->SetActive(true);
			p->Play();
		}

		{
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\EnemyCharge1.yaml");
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(0.5, 0.5, 1))
				* rotationMT
				* Matrix::CreateTranslation(effectPos)
			);

			p->SetSimulationSpeed(2.5f);
			p->SetActive(true);
			p->Play();
		}

		m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\02 Combat_Sound\\Player_Swing_1_Sound.wav", true, 38);
	}
}

void EnemyAnimator::SoundPlay(std::wstring path, bool isDup, int channel)
{
	m_managers.lock()->Sound()->Play(path, isDup, channel);
}

