#include "BossAnimator.h"
#include "SkinnedMesh.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "PlayerAnimator.h"
#include <random>

BOOST_CLASS_EXPORT_IMPLEMENT(BossAnimator)

int BossAnimator::RandomNumber(int _min, int _max)
{
	std::random_device rd;
	// 시드값으로 난수 생성 엔진 초기화
	std::mt19937 gen(rd());
	// 균등 분포 정의
	std::uniform_int_distribution<int> dis(_min, _max);
	// 난수 반환
	return dis(gen);
}

BossAnimator::BossAnimator()
	: m_isPursuit(false)
	, m_strafeMove(false)
	, m_isReturn(false)
	, m_isInRange(false)
	, m_attackSwing(false)
	, m_attackRunning(false)
	, m_attackUpperCut(false)
	, m_attackChargeCombo(false)
	, m_attackSmashGround(false)
	, m_attackCharge(false)
	, m_attackJumpSmashGround(false)
	, m_attackSpin(false)
	, m_attackDoubleUpperCut(false)
	, m_attackCombo1_1(false)
	, m_attackCombo1_2(false)
	, m_attackCombo1_3(false)
	, m_attackCombo2_1(false)
	, m_attackCombo2_2(false)
	, m_attackSwordShoot(false)
	, m_attackShockWave(false)
	, m_isDamage(false)
	, m_isDown(false)
	, m_isDeath(false)
	, m_isSkillActive(false)
	, m_isAttacking(false)
	, m_playOnce(false)
	, m_isAnimationEnd(false)
	, m_skillCoolTime(false)
	, m_passingTime(0.f)
	, m_lastHp(0.f)
	, m_hitStopTime(0.f)
	, m_sideMove(0.f)
	, m_downGuage(300.f)
	, m_currentFrame(0)
	, m_currentPhase(0)
	, m_currentState(nullptr)
{

}

BossAnimator::~BossAnimator()
{

}

void BossAnimator::Awake()
{
	m_animationStateMap["Idle"] = new BossIdle(this);
	m_animationStateMap["Run"] = new BossRun(this);
	m_animationStateMap["Strafe"] = new BossStrafe(this);
	m_animationStateMap["Dodge"] = new BossDodge(this);
	m_animationStateMap["Down"] = new BossDown(this);
	m_animationStateMap["AttackSwing"] = new BossAttackSwing(this);
	m_animationStateMap["AttackRunning"] = new BossAttackRunning(this);
	m_animationStateMap["AttackUpperCut"] = new BossAttackUpperCut(this);
	m_animationStateMap["AttackChargeCombo"] = new BossAttackChargeCombo(this);
	m_animationStateMap["AttackSmashGround"] = new BossAttackSmashGround(this);
	m_animationStateMap["AttackCharge"] = new BossAttackCharge(this);
	m_animationStateMap["AttackJumpSmashGround"] = new BossAttackJumpSmashGround(this);
	m_animationStateMap["AttackSpin"] = new BossAttackSpin(this);
	m_animationStateMap["AttackDoubleUpperCut"] = new BossAttackDoubleUpperCut(this);
	m_animationStateMap["AttackCombo1_1"] = new BossAttackCombo1_1(this);
	m_animationStateMap["AttackCombo1_2"] = new BossAttackCombo1_2(this);
	m_animationStateMap["AttackCombo1_3"] = new BossAttackCombo1_3(this);
	m_animationStateMap["AttackCombo2_1"] = new BossAttackCombo2_1(this);
	m_animationStateMap["AttackCombo2_2"] = new BossAttackCombo2_2(this);
	m_animationStateMap["AttackSwordShoot"] = new BossAttackSwordShoot(this);
	m_animationStateMap["AttackShockWave"] = new BossAttackShockWave(this);


	m_currentState = m_animationStateMap["Idle"];
}

void BossAnimator::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_enemyController = m_owner.lock().get()->GetComponent<EnemyController>().lock();
	m_enemy = m_owner.lock().get()->GetComponent<Enemy>().lock();
	auto playerEntity = m_enemy->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.get()).Get().lock();
	m_playerAnimator = playerEntity->GetComponent<PlayerAnimator>().lock();

	m_skinnedMesh->AddAnimation("BossIdle", L"BossAnimations/Idle/Idle");
	m_skinnedMesh->AddAnimation("BossAttackLightSpeedReady", L"BossAnimations/Attacks/AttackLightSpeedReady");
	m_skinnedMesh->AddAnimation("BossAttackLightSpeedDash", L"BossAnimations/Attacks/AttackLightSpeedDash");
	m_skinnedMesh->AddAnimation("BossAttackChargedCombo", L"BossAnimations/Attacks/AttackChargedCombo");
	m_skinnedMesh->AddAnimation("BossAttackJumpSmashGround", L"BossAnimations/Attacks/AttackJumpSmashGround");
	m_skinnedMesh->AddAnimation("BossAttackRun", L"BossAnimations/Attacks/AttackRun");
	m_skinnedMesh->AddAnimation("BossAttackShockWave", L"BossAnimations/Attacks/AttackShockWave");
	m_skinnedMesh->AddAnimation("BossAttackSmashGround", L"BossAnimations/Attacks/AttackSmashGround");
	m_skinnedMesh->AddAnimation("BossAttackSpin", L"BossAnimations/Attacks/AttackSpin");
	m_skinnedMesh->AddAnimation("BossAttackSwing", L"BossAnimations/Attacks/AttackSwing");
	m_skinnedMesh->AddAnimation("BossAttackUpperCut", L"BossAnimations/Attacks/AttackUpperCut");
	m_skinnedMesh->AddAnimation("BossAttackUpperCutMirror", L"BossAnimations/Attacks/AttackUpperCutMirror");
	m_skinnedMesh->AddAnimation("BossAttackCombo1_1", L"BossAnimations/Attacks/Combo1-1");
	m_skinnedMesh->AddAnimation("BossAttackCombo1_2", L"BossAnimations/Attacks/Combo1-2");
	m_skinnedMesh->AddAnimation("BossAttackCombo1_3", L"BossAnimations/Attacks/Combo1-3");
	m_skinnedMesh->AddAnimation("BossAttackCombo2_1", L"BossAnimations/Attacks/Combo2-1");
	m_skinnedMesh->AddAnimation("BossAttackCombo2_2", L"BossAnimations/Attacks/Combo2-2");
	m_skinnedMesh->AddAnimation("BossDodge", L"BossAnimations/Dodge/Dodge");
	m_skinnedMesh->AddAnimation("BossDodgeLeft", L"BossAnimations/Dodge/DodgeLeft");
	m_skinnedMesh->AddAnimation("BossDodgeRight", L"BossAnimations/Dodge/DodgeRight");
	m_skinnedMesh->AddAnimation("BossRun", L"BossAnimations/Run/Run");
	m_skinnedMesh->AddAnimation("BossStrafeL", L"BossAnimations/Strafe/StrafeL");
	m_skinnedMesh->AddAnimation("BossStrafeR", L"BossAnimations/Strafe/StrafeR");
	m_skinnedMesh->AddAnimation("BossDown1", L"BossAnimations/Down/Down1");
	m_skinnedMesh->AddAnimation("BossDown2", L"BossAnimations/Down/Down2");
	m_skinnedMesh->AddAnimation("BossDown3", L"BossAnimations/Down/Down3");


	m_currentState->OnStateEnter();
}

void BossAnimator::Update()
{
	if (m_isDeath || !m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.get()).Get())
	{
		return;
	}

	if (m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() <= 0.f)
	{
		m_isDeath = true;
	}

	m_sideMove = m_enemyController->GetTypeInfo().GetProperty("sideMove")->Get<float>(m_enemyController.get()).Get();
	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();
	m_currentFrame = m_skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(m_skinnedMesh.get()).Get();
	m_isInRange = m_enemyController->GetTypeInfo().GetProperty("isAttackReady")->Get<bool>(m_enemyController.get()).Get();
	m_enemyController->GetTypeInfo().GetProperty("strafeMove")->Set(m_enemyController.get(), m_strafeMove);

	float currentTP = m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get();
	float maxTP = m_enemy->GetTypeInfo().GetProperty("maxTP")->Get<float>(m_enemy.get()).Get();

	int random = RandomNumber(1, 20);

	// 스턴용
	if (m_enemy->GetTypeInfo().GetProperty("stunGuage")->Get<float>(m_enemy.get()).Get() > m_downGuage)
	{
		m_enemy->GetTypeInfo().GetProperty("stunGuage")->Set(m_enemy.get(), 0.f);
		m_downGuage += 100.f;
		m_isDown = true;
	}

	if (m_isDown)
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 2.6f)
		{
			m_isDown = false;
			m_passingTime = 0.f;
		}
	}

	// 페이즈 처리
	if (currentTP / maxTP > 0.7f && m_currentPhase == 0)
	{
		m_playOnce = false;
		m_currentPhase = 1;
	}
	else if (currentTP / maxTP <= 0.7f && currentTP / maxTP > 0.3f && m_currentPhase == 1)
	{
		m_playOnce = false;
		m_currentPhase = 2;
	}
	else if (currentTP / maxTP < 0.3f && m_currentPhase == 2)
	{
		m_playOnce = false;
		m_currentPhase = 3;
	}

	if (m_currentState == m_animationStateMap["Idle"])
	{
		m_passingTime = 0.f;
	}

	/// 각 페이즈 별 조건처리
	if (m_currentPhase == 1)
	{
		if (!m_playOnce)
		{
			m_enemy->GetTypeInfo().GetProperty("stunGuage")->Set(m_enemy.get(), 0.f);

			m_attackRunning = true;
			m_playOnce = true;
			m_passingTime = 0.f;
		}
		else
		{
			if (!m_isAttacking)
			{
				m_passingTime += GetDeltaTime();

				if (m_isInRange)
				{
					if (m_attackCombo1_1)
					{
						if (random <= 10)
						{
							m_attackCombo1_2 = true;
							m_attackCombo1_3 = false;
						}
						else
						{
							m_attackSpin = true;
						}
					}
					if (m_attackCombo2_1)
					{
						m_attackCombo2_2 = true;
					}
					if (m_passingTime < 1.f)
					{
						m_strafeMove = true;
					}
					else
					{
						m_strafeMove = false;
						if (random <= 10)
						{
							m_attackCombo1_1 = true;
						}
						else
						{
							m_attackCombo2_1 = true;
						}
					}
				}
				else
				{
					if (m_attackCombo1_1)
					{
						m_attackCombo1_3 = true;
						m_attackCombo1_2 = false;
						m_attackSpin = false;
					}
					if (!m_skillCoolTime)
					{
						if (random <= 7)
						{
							m_attackSwordShoot = true;
						}
						else if (random > 7 && random <= 14)
						{
							m_attackShockWave = true;
						}
						else
						{
							m_attackCharge = true;
						}
					}
					else
					{
						m_strafeMove = true;
					}
				}
			}

			if (m_attackCharge)
			{
				m_passingTime += GetDeltaTime();
			}
		}
	}
	else if (m_currentPhase == 2)
	{
		if (!m_playOnce)
		{
			m_playOnce = true;
			m_enemy->GetTypeInfo().GetProperty("stunGuage")->Set(m_enemy.get(), 0.f);
		}
		else
		{

		}
	}
	else if (m_currentPhase == 3)
	{
		if (!m_playOnce)
		{
			m_enemy->GetTypeInfo().GetProperty("stunGuage")->Set(m_enemy.get(), 0.f);
			m_isDown = true;
			if (m_currentState == m_animationStateMap["Down"])
			{
				m_enemy->GetTypeInfo().GetProperty("currentTP")->Set(m_enemy.get(), currentTP + GetDeltaTime() * 5.f);
			}
			if (currentTP / maxTP >= 0.5f)
			{
				m_playOnce = true;
				m_isDown = false;
			}
		}
		else
		{

		}
	}



	if (!m_attackCharge)
	{
		if (m_isAttacking)
		{
			m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
		}
		else
		{
			m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
		}
	}

	m_currentState->OnStateUpdate();

}

void BossAnimator::SetCanMove(bool canMove)
{
	m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), canMove);
}

void BossAnimator::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
}

void BossAnimator::SetAnimationSpeed(float speed)
{
	m_skinnedMesh->SetAnimationSpeed(speed);
}

void BossAnimator::SetAnimationPlay(bool playStop)
{
	m_skinnedMesh->SetPlayStop(playStop);
}

void BossAnimator::ChangeState(std::string stateName)
{
	m_currentState->OnStateExit();
	m_currentState = m_animationStateMap[stateName];
	m_currentState->OnStateEnter();
}

void BossAnimator::SetEnemyDamage(float damage)
{
	m_enemy->GetTypeInfo().GetProperty("currentDamage")->Set(m_enemy.get(), damage);
}

void BossIdle::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossIdle", false);
	GetProperty("isAttacking")->Set(m_animator, false);
}

void BossIdle::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		// 		else if (!GetProperty("isInRange")->Get<bool>(m_animator).Get())
		// 		{
		// 			dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Run");
		// 		}
	}
	if (GetProperty("strafeMove")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Strafe");
	}
	if (GetProperty("isPursuit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Run");
	}
	if (GetProperty("attackSwing")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackSwing");
	}
	if (GetProperty("attackRunning")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackRunning");
	}
	if (GetProperty("attackUpperCut")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackUpperCut");
	}
	if (GetProperty("attackChargeCombo")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackChargeCombo");
	}
	if (GetProperty("attackSmashGround")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackSmashGround");
	}
	if (GetProperty("attackCharge")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCharge");
	}
	if (GetProperty("attackJumpSmashGround")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackJumpSmashGround");
	}
	if (GetProperty("attackDoubleUpperCut")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackDoubleUpperCut");
	}
	if (GetProperty("attackCombo1_1")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo1_1");
	}
	if (GetProperty("attackCombo2_1")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo2_1");
	}
	if (GetProperty("attackSwordShoot")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackSwordShoot");
	}
	if (GetProperty("attackShockWave")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackShockWave");
	}
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}

}

void BossRun::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossRun", false);
}

void BossRun::OnStateUpdate()
{
	if (GetProperty("isInRange")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("BossAttackRunning");
	}
}

void BossStrafe::OnStateEnter()
{
	m_isChangePose = true;
}

void BossStrafe::OnStateUpdate()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(true);
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
			dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossStrafeR", false);
		}
		else
		{
			dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossStrafeL", false);
		}
	}
	if (GetProperty("passingTime")->Get<float>(m_animator).Get() > 1.f)
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
		//GetProperty("passingTime")->Set(m_animator, 0.f);
	}
}

void BossDodge::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossDodge", false);
}

void BossDodge::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackSwing::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackSwing", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackSwing::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackSwing::OnStateExit()
{
	GetProperty("attackSwing")->Set(m_animator, false);
}

void BossAttackRunning::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackRun", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackRunning::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
		dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(false);
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 17)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(0.f);
		dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(true);
	}
	if (GetProperty("isInRange")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(false);
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(1.f);
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackRunning::OnStateExit()
{
	GetProperty("attackRunning")->Set(m_animator, false);
	isReset = false;
}

void BossAttackUpperCut::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackUpperCut", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackUpperCut::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackUpperCut::OnStateExit()
{
	GetProperty("attackUpperCut")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("skillCoolTime")->Set(m_animator, true);
}

void BossAttackChargeCombo::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackChargeCombo", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackChargeCombo::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackChargeCombo::OnStateExit()
{
	GetProperty("attackChargeCombo")->Set(m_animator, false);
}

void BossAttackSmashGround::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackSmashGround", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackSmashGround::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackSmashGround::OnStateExit()
{
	GetProperty("attackSmashGround")->Set(m_animator, false);
}

void BossAttackCharge::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackLightSpeedReady", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(false);
}

void BossAttackCharge::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (m_changePose && isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 8)
	{
		//dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
		dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(false);
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(0.f);
	}
	if (m_changePose && isReset && GetProperty("passingTime")->Get<float>(m_animator).Get() > 3.f)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(1.f);
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
	if (!m_changePose && isReset&&GetProperty("currentFrame")->Get<int>(m_animator).Get() > 142)
	{
		m_changePose = true;
		isReset = false;
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackLightSpeedDash", false);
		dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(true);
		GetProperty("passingTime")->Set(m_animator, 0.f);
	}
}

void BossAttackCharge::OnStateExit()
{
	GetProperty("attackCharge")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	m_changePose = false;
	isReset = false;
}

void BossAttackJumpSmashGround::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackJumpSmashGround", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackJumpSmashGround::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackJumpSmashGround::OnStateExit()
{
	GetProperty("attackJumpSmashGround")->Set(m_animator, false);
}

void BossAttackSpin::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackSpin", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackSpin::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackSpin::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackSpin")->Set(m_animator, false);
}

void BossAttackDoubleUpperCut::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackUpperCut", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackDoubleUpperCut::OnStateUpdate()
{
	if (!m_isChangePose)
	{
		if (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 114)
		{
			dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackUpperCutMirror", false);
			m_isChangePose = true;
		}
	}
	if (m_isChangePose && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackDoubleUpperCut::OnStateExit()
{
	m_isChangePose = false;
	GetProperty("attackDoubleUpperCut")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("skillCoolTime")->Set(m_animator, true);
}

void BossDown::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossDown1", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossDown::OnStateUpdate()
{
	if (!m_isChangePose)
	{
		if (GetProperty("isDown")->Get<bool>(m_animator).Get() && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 59)
		{
			dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossDown2", false);
			m_isChangePose = true;
		}
		else if (!GetProperty("isDown")->Get<bool>(m_animator).Get())
		{
			if (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 85)
			{
				dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
			}
		}
	}
	if (m_isChangePose)
	{
		if (!GetProperty("isDown")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossDown3", false);
			m_isChangePose = false;
		}
	}
}

void BossDown::OnStateExit()
{
	m_isChangePose = false;
}

void BossAttackCombo1_1::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo1_1", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackCombo1_1::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("attackCombo1_2")->Get<bool>(m_animator).Get() && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 38)
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo1_2");
	}
	else if (isReset && GetProperty("attackCombo1_3")->Get<bool>(m_animator).Get() && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 45)
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo1_3");
	}
	else if (isReset && GetProperty("attackSpin")->Get<bool>(m_animator).Get() && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 48)
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackSpin");
	}
}

void BossAttackCombo1_1::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackCombo1_1")->Set(m_animator, false);
}

void BossAttackCombo1_2::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo1_2", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackCombo1_2::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackCombo1_2::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackCombo1_2")->Set(m_animator, false);
}

void BossAttackCombo1_3::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo1_3", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackCombo1_3::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("attackCombo1_3")->Get<bool>(m_animator).Get() && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 34)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(true);
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackCombo1_3::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackCombo1_3")->Set(m_animator, false);
}

void BossAttackCombo2_1::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo2_1", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackCombo2_1::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("attackCombo2_2")->Get<bool>(m_animator).Get() && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 47)
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo2_2");
	}
}

void BossAttackCombo2_1::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackCombo2_1")->Set(m_animator, false);
}

void BossAttackCombo2_2::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo2_2", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackCombo2_2::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackCombo2_2::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackCombo2_2")->Set(m_animator, false);
}

void BossAttackSwordShoot::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo1_1", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackSwordShoot::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 20)
	{
		GetProperty("isSkillActive")->Set(m_animator, true);
	}
	if (isReset && !GetProperty("isAttacking")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackSwordShoot::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackSwordShoot")->Set(m_animator, false);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("skillCoolTime")->Set(m_animator, true);
}

void BossAttackShockWave::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackShockWave", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackShockWave::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 34)
	{
		GetProperty("isSkillActive")->Set(m_animator, true);
	}
	if (isReset && !GetProperty("isAttacking")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackShockWave::OnStateExit()
{
	GetProperty("attackShockWave")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("skillCoolTime")->Set(m_animator, true);
}
