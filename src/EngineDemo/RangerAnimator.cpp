#include "RangerAnimator.h"
#include "SkinnedMesh.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "PlayerAnimator.h"
#include "Transform.h"
#include <random>
#include "ParticleManager.h"
#include "IParticleSystem.h"
#include "SoundManager.h"

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
	, m_isBack(false)
	, m_isFall(false)
	, m_isDeath(false)
	, m_isReload(false)
	, m_isShooting(false)
	, m_isAnimationEnd(false)
	, m_shootReady(false)
	, m_showEffect(false)
	, m_passingTime(0.f)
	, m_lastHp(0.f)
	, m_baseSpeed(0.f)
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

	m_animationStateMap["Fall"] = new RangerFall(this);

	m_animationStateMap["KnockBack"] = new RangerKnockBack(this);

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
	m_baseSpeed = m_enemy->GetTypeInfo().GetProperty("speed")->Get<float>(m_enemy.get()).Get();

	m_skinnedMesh->AddAnimation("EnemyRangeAim", L"EnemyAnimations/RangeEnemy/Aim/Aim");
	m_skinnedMesh->AddAnimation("EnemyRangeAttack", L"EnemyAnimations/RangeEnemy/Attack/Attack");
	m_skinnedMesh->AddAnimation("EnemyDown", L"EnemyAnimations/MeleeEnemy/Down/Down");
	m_skinnedMesh->AddAnimation("EnemyFall", L"EnemyAnimations/MeleeEnemy/Down/Fall");
	m_skinnedMesh->AddAnimation("EnemyFallAttack", L"EnemyAnimations/MeleeEnemy/Down/FallAttack");
	m_skinnedMesh->AddAnimation("EnemyGetUp", L"EnemyAnimations/MeleeEnemy/Down/GetUp");
	m_skinnedMesh->AddAnimation("EnemyKnockBack", L"EnemyAnimations/MeleeEnemy/Down/KnockBack");
	m_skinnedMesh->AddAnimation("EnemyRangeHit", L"EnemyAnimations/RangeEnemy/Hit/Hit");
	m_skinnedMesh->AddAnimation("EnemyRangeIdle", L"EnemyAnimations/RangeEnemy/Idle/Idle");
	m_skinnedMesh->AddAnimation("EnemyRangePursuitReturn", L"EnemyAnimations/RangeEnemy/Pursuit Return/PursuitReturn");
	m_skinnedMesh->AddAnimation("EnemyRangeReload", L"EnemyAnimations/RangeEnemy/Reload/Reload");

	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Bow_load_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Bow_Shoot_Sound.wav", false);

	m_magicCircle = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\MagicCircle.yaml");
	m_shootEffect = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ShootImpact.yaml");

	m_currentState->OnStateEnter();
}

void RangerAnimator::Update()
{
	if (m_isDeath)
	{
		return;
	}



	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();
	m_currentFrame = m_skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(m_skinnedMesh.get()).Get();
	m_isPursuit = m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.get()).Get();
	m_isAttack = m_enemyController->GetTypeInfo().GetProperty("isAttackReady")->Get<bool>(m_enemyController.get()).Get();
	m_isReturn = m_enemyController->GetTypeInfo().GetProperty("isComeBack")->Get<bool>(m_enemyController.get()).Get();

	PlayEffect();

	if (m_enemy->GetTypeInfo().GetProperty("slowTime")->Get<bool>(m_enemy.get()).Get())
	{
		m_skinnedMesh->SetAnimationSpeed(0.3f);
	}
	else
	{
		m_skinnedMesh->SetAnimationSpeed(1.f);
	}

	if (m_isReload)
	{
		m_passingTime += GetDeltaTime();
	}

	if (m_isAttack || m_currentState == m_animationStateMap["Aim"] || m_currentState == m_animationStateMap["Reload"] || m_currentState == m_animationStateMap["Attack"]
		|| m_currentState == m_animationStateMap["Damage"] || m_currentState == m_animationStateMap["Down"] || m_currentState == m_animationStateMap["Fall"] || m_currentState == m_animationStateMap["KnockBack"])
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


	if (m_lastHp > m_enemy->GetCurrentTP() && m_enemy->GetCurrentTP() > 0.f)
	{
		m_isDamage = true;
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);

		if (m_playerAnimator->GetBackAttack())
		{
			int random = RandomNumber(1, 4);

			if (random == 1)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_1_Sound.wav", false, 45);
			else if (random == 2)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_2_Sound.wav", false, 45);
			else if (random == 3)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_3_Sound.wav", false, 45);
			else if (random == 4)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_4_Sound.wav", false, 45);

			m_isBack = true;
			m_isDamage = false;
		}
		else if (m_playerAnimator->GetFallAttack())
		{
			int random = RandomNumber(1, 2);

			if (random == 1)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_5_Sound.wav", false, 46);
			else if (random == 2)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_6_Sound.wav", false, 46);


			m_isFall = true;
			m_isDamage = false;
		}
		else if (m_playerAnimator->GetDownAttack())
		{
			int random = RandomNumber(1, 2);

			if (random == 1)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_5_Sound.wav", false, 47);
			else if (random == 2)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_6_Sound.wav", false, 47);

			m_isDown = true;
			m_isDamage = false;
		}
		else
		{
			int random = RandomNumber(1, 4);

			if (random == 1)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_1_Sound.wav", false, 48);
			else if (random == 2)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_2_Sound.wav", false, 48);
			else if (random == 3)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_3_Sound.wav", false, 48);
			else if (random == 4)
				m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Pain_4_Sound.wav", false, 48);
		}
	}

	if (m_isPursuit)
	{
		m_enemy->GetTypeInfo().GetProperty("speed")->Set(m_enemy.get(), m_baseSpeed * 1.5f);
	}
	else
	{
		m_enemy->GetTypeInfo().GetProperty("speed")->Set(m_enemy.get(), m_baseSpeed);
	}

	m_lastHp = m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get();

	m_currentState->OnStateUpdate();

	if (m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() <= 0.f)
	{
		m_shootReady = false;
		m_showEffect = false;
		m_isDeath = true;
		PlayEffect();
	}
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

void RangerAnimator::SetImpulse(float power)
{
	m_enemyController->GetTypeInfo().GetProperty("useImpulse")->Set(m_enemyController.get(), true);
	m_enemyController->GetTypeInfo().GetProperty("impulsePower")->Set(m_enemyController.get(), power);
}

void RangerAnimator::SoundPlay(std::wstring path, bool isDup, int channel)
{
	m_managers.lock()->Sound()->Play(path, isDup, channel);
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
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("KnockBack");
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
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 6 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 16)
	{
		dynamic_cast<RangerAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Enemy\\Enemy_Walk_1_Sound.wav", true, 51);
	}

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
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("KnockBack");
	}
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}

	if (GetProperty("isReturn")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Return");
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
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("KnockBack");
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
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 9)
	{
		GetProperty("isShooting")->Set(m_animator, true);
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 43)
	{
		GetProperty("showEffect")->Set(m_animator, false);
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
		else if (GetProperty("isReturn")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Return");
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
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("KnockBack");
	}
	else if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Death");
	}
}

void RangerAttack::OnStateExit()
{

}

void RangerAim::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyRangeAim", false);
	GetProperty("shootReady")->Set(m_animator, true);
}

void RangerAim::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 127)
	{
		GetProperty("shootReady")->Set(m_animator, true);
	}
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
	else if (GetProperty("isReturn")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Return");
	}
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Damage");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("KnockBack");
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
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 17 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 40)
	{
		dynamic_cast<RangerAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Enemy\\Enemy_Walk_1_Sound.wav", true, 51);
	}

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
	else if (GetProperty("isReturn")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Return");
	}
	else if (GetProperty("isDamage")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Damage");
	}
	else if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("KnockBack");
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
	else if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Fall");
	}
	else if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("KnockBack");
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
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyDown", false);
	GetProperty("isDown")->Set(m_animator, false);
}

void RangerDown::OnStateUpdate()
{
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isChange = true;
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		isReset = true;
		dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyGetUp", false);
	}
	if (isChange && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Aim");
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
	isChange = false;
	isReset = false;
}

void RangerFall::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyFall", false);
	GetProperty("isFall")->Set(m_animator, false);
}

void RangerFall::OnStateUpdate()
{
	if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		GetProperty("isFall")->Set(m_animator, false);
		dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyFallAttack", false);
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
}

void RangerFall::OnStateExit()
{
	GetProperty("isFall")->Set(m_animator, false);
}

void RangerKnockBack::OnStateEnter()
{
	dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyKnockBack", false);
	GetProperty("isBack")->Set(m_animator, false);
	dynamic_cast<RangerAnimator*>(m_animator)->SetImpulse(-20.f);
}

void RangerKnockBack::OnStateUpdate()
{
	if (GetProperty("isBack")->Get<bool>(m_animator).Get())
	{
		GetProperty("isBack")->Set(m_animator, false);
		dynamic_cast<RangerAnimator*>(m_animator)->SetAnimation("EnemyKnockBack", false);
	}
	if (GetProperty("isFall")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Fall");
	}
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Down");
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<RangerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void RangerKnockBack::OnStateExit()
{

}

void RangerDeath::OnStateEnter()
{

}

void RangerDeath::OnStateUpdate()
{

}


void RangerAnimator::PlayEffect()
{
	Vector3 effectPos = m_owner.lock()->GetWorldPosition();
	Vector3 effectRot = m_owner.lock()->GetWorldRotation().ToEuler();
	effectPos.y += 1.3f;

	{

		Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));
		Matrix m = Matrix::CreateScale(Vector3(1, 1, 1)) * rotationMT * Matrix::CreateTranslation(effectPos);

		m_magicCircle->SetTransformMatrix(
			Matrix::CreateScale(0.5f)
			* Matrix::CreateRotationX(1.57f)
			* Matrix::CreateTranslation(Vector3(0.f, 0.f, -1.2f))
			* m
		);

		if (m_shootReady && !m_showEffect)
		{
			m_shootReady = false;
			m_showEffect = true;

			m_magicCircle->SetActive(true);
			m_magicCircle->Play();
		}
		else if (m_shootReady && m_showEffect)
		{
			m_shootReady = false;
			m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Bow_load_Sound.wav", true, 49);
		}

	}

	if (m_isShooting)
	{
		m_isShooting = false;

		Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));
		Matrix m = Matrix::CreateScale(Vector3(1, 1, 1)) * rotationMT * Matrix::CreateTranslation(effectPos);

		m_shootEffect->SetTransformMatrix(Matrix::CreateScale(0.8f)
			* Matrix::CreateRotationX(1.57f)
			* Matrix::CreateRotationY(-1.57f)
			* Matrix::CreateTranslation(Vector3(0, 0, -1.2f))
			* m
		);


		m_shootEffect->SetActive(true);
		m_shootEffect->Play();


		m_managers.lock()->Sound()->Play(L"..\\Resources\\Sounds\\08. Enemy_Sound\\Enemy_Bow_Shoot_Sound.wav", true, 50);

	}

	if (!m_showEffect)
	{
		m_magicCircle->SetActive(false);
		m_shootEffect->SetActive(false);
	}
}
