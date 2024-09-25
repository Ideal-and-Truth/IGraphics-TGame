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
	, m_isDamage(false)
	, m_isDown(false)
	, m_isDeath(false)
	, m_isAnimationEnd(false)
	, m_passingTime(0.f)
	, m_lastHp(0.f)
	, m_hitStopTime(0.f)
	, m_sideMove(0.f)
	, m_attackCoolTime(0.f)
	, m_currentFrame(0)
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
	m_animationStateMap["AttackSwing"] = new BossAttackSwing(this);
	m_animationStateMap["AttackRunning"] = new BossAttackRunning(this);
	m_animationStateMap["AttackUpperCut"] = new BossAttackUpperCut(this);
	m_animationStateMap["AttackChargeCombo"] = new BossAttackChargeCombo(this);
	m_animationStateMap["AttackSmashGround"] = new BossAttackSmashGround(this);
	m_animationStateMap["AttackCharge"] = new BossAttackCharge(this);
	m_animationStateMap["AttackJumpSmashGround"] = new BossAttackJumpSmashGround(this);
	m_animationStateMap["AttackSpin"] = new BossAttackSpin(this);
	m_animationStateMap["AttackDoubleUpperCut"] = new BossAttackDoubleUpperCut(this);


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
	m_skinnedMesh->AddAnimation("BossAttackCharge", L"BossAnimations/Attacks/AttackCharge");
	m_skinnedMesh->AddAnimation("BossAttackChargedCombo", L"BossAnimations/Attacks/AttackChargedCombo");
	m_skinnedMesh->AddAnimation("BossAttackJumpSmashGround", L"BossAnimations/Attacks/AttackJumpSmashGround");
	m_skinnedMesh->AddAnimation("BossAttackRunningSimple", L"BossAnimations/Attacks/AttackRunningSimple");
	m_skinnedMesh->AddAnimation("BossAttackSmashGround", L"BossAnimations/Attacks/AttackSmashGround");
	m_skinnedMesh->AddAnimation("BossAttackSpin", L"BossAnimations/Attacks/AttackSpin");
	m_skinnedMesh->AddAnimation("BossAttackSwing", L"BossAnimations/Attacks/AttackSwing");
	m_skinnedMesh->AddAnimation("BossAttackUpperCut", L"BossAnimations/Attacks/AttackUpperCut");
	m_skinnedMesh->AddAnimation("BossAttackUpperCutMirror", L"BossAnimations/Attacks/AttackUpperCutMirror");
	m_skinnedMesh->AddAnimation("BossDodge", L"BossAnimations/Dodge/Dodge");
	m_skinnedMesh->AddAnimation("BossRun", L"BossAnimations/Run/Run");
	m_skinnedMesh->AddAnimation("BossStrafeL", L"BossAnimations/Strafe/StrafeL");
	m_skinnedMesh->AddAnimation("BossStrafeR", L"BossAnimations/Strafe/StrafeR");


	m_currentState->OnStateEnter();
}

void BossAnimator::Update()
{
	if (m_isDeath)
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
	m_isPursuit = m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.get()).Get();
	m_isInRange = m_enemyController->GetTypeInfo().GetProperty("isAttackReady")->Get<bool>(m_enemyController.get()).Get();
	m_enemyController->GetTypeInfo().GetProperty("strafeMove")->Set(m_enemyController.get(), m_strafeMove);

	float currentTP = m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get();
	float maxTP = m_enemy->GetTypeInfo().GetProperty("maxTP")->Get<float>(m_enemy.get()).Get();

	int random = RandomNumber(1, 20);

	if (GetKeyDown(KEY::Q))
	{
		m_attackUpperCut = true;
	}
	/// 이전 패턴의 죄
	/*if (!m_attackSwing && !m_attackRunning && !m_attackUpperCut && !m_attackChargeCombo && !m_attackSmashGround && !m_attackCharge && !m_attackJumpSmashGround && !m_attackSpin && !m_attackDoubleUpperCut)
	{
		m_attackCoolTime += GetDeltaTime();
	}

	// 1페이즈
	if (currentTP / maxTP > 0.7f)
	{
		if (!m_attackSwing && !m_attackRunning && !m_attackUpperCut && !m_attackChargeCombo && !m_attackSmashGround && !m_attackCharge && !m_attackJumpSmashGround && !m_attackSpin && !m_attackDoubleUpperCut)
		{
			if (m_attackCoolTime < 10.f && !m_isInRange)
			{
				m_strafeMove = true;
			}
			if (m_attackCoolTime > 10.f && !m_strafeMove)
			{
				// 범위 안이면
				if (m_isInRange)
				{
					// 75% 휘두르기
					if (random >= 1 && random <= 15)
					{
						m_attackSwing = true;
						m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
					}
					// 25% 달리기공격
					else if (random >= 16 && random <= 20)
					{
						m_attackRunning = true;
						m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
					}
					m_attackCoolTime = 0.f;
				}
				else
				{
					m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
				}
			}

			if (m_currentState == m_animationStateMap["Strafe"])
			{
				// 5초 배회
				m_passingTime += GetDeltaTime();

				if (m_passingTime > 5.f)
				{
					// 범위 안이면
					if (m_isInRange)
					{
						m_strafeMove = false;
					}
					// 범위 밖이면
					else
					{
						// 50% 어퍼컷
						if (random >= 1 && random <= 10)
						{
							m_attackUpperCut = true;
							m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
						}
						// 25% 땅 내리찍기
						else if (random >= 11 && random <= 15)
						{
							m_attackSmashGround = true;
							m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
						}
						// 25% 리사이클
						else if (random >= 16 && random <= 20)
						{
							// 다시 바라보기
						}

						m_strafeMove = false;
					}
				}
				else
				{
					m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
				}
			}
		}
	}
	// 2페이즈
	else if (currentTP / maxTP < 0.7f && currentTP / maxTP > 0.3f)
	{
		if (!m_attackSwing && !m_attackRunning && !m_attackUpperCut && !m_attackChargeCombo && !m_attackSmashGround && !m_attackCharge && !m_attackJumpSmashGround && !m_attackSpin && !m_attackDoubleUpperCut)
		{
			if (m_attackCoolTime < 10.f && !m_isInRange)
			{
				m_strafeMove = true;
			}
			if (m_attackCoolTime > 10.f && !m_strafeMove)
			{
				// 범위 안이면
				if (m_isInRange)
				{
					// 50% 휘두르기
					if (random >= 1 && random <= 10)
					{
						m_attackSwing = true;
						m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
					}
					// 25% 차지콤보
					else if (random >= 11 && random <= 15)
					{
						m_attackChargeCombo = true;
						m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
					}
					// 25% 달리기 공격
					else if (random >= 16 && random <= 20)
					{
						m_attackRunning = true;
						m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
					}
					m_attackCoolTime = 0.f;
				}
				else
				{
					m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
				}
			}

			if (m_currentState == m_animationStateMap["Strafe"])
			{
				// 5초 배회
				m_passingTime += GetDeltaTime();

				if (m_passingTime > 5.f)
				{
					// 범위 안이면
					if (m_isInRange)
					{
						m_strafeMove = false;
					}
					// 범위 밖이면
					else
					{
						// 30% 돌진
						if (random >= 1 && random <= 6)
						{
							m_attackCharge = true;
							m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
						}
						// 40% 어퍼컷
						else if (random >= 7 && random <= 14)
						{
							m_attackUpperCut = true;
							m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
						}
						// 30% 내리찍기
						else if (random >= 15 && random <= 20)
						{
							m_attackSmashGround = true;
							m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
						}

						m_strafeMove = false;
					}
				}
				else
				{
					m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
				}
			}
		}
	}
	// 3페이즈
	else if (currentTP / maxTP < 0.3f)
	{

	}


	if (!m_attackSwing && !m_attackRunning && !m_attackUpperCut && !m_attackChargeCombo && !m_attackSmashGround && !m_attackCharge && !m_attackJumpSmashGround && !m_attackSpin && !m_attackDoubleUpperCut)
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
	}

	// 바라만 보다가
	if (!m_isAnimationEnd && m_currentState == m_animationStateMap["Idle"])
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
	}*/

	m_currentState->OnStateUpdate();
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
}

void BossIdle::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("strafeMove")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Strafe");
		}
		else if (!GetProperty("isInRange")->Get<bool>(m_animator).Get() && GetProperty("attackCoolTime")->Get<float>(m_animator).Get() > 3.f)
		{
			dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Run");
		}
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
	if (GetProperty("attackSpin")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackSpin");
	}
	if (GetProperty("attackDoubleUpperCut")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackDoubleUpperCut");
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
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossStrafe::OnStateEnter()
{
	m_isChangePose = true;
}

void BossStrafe::OnStateUpdate()
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
			dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossStrafeR", false);
		}
		else
		{
			dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossStrafeL", false);
		}
	}
	if (GetProperty("passingTime")->Get<float>(m_animator).Get() > 5.f)
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
		GetProperty("passingTime")->Set(m_animator, 0.f);
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
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackRunningSimple", false);
}

void BossAttackRunning::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackRunning::OnStateExit()
{
	GetProperty("attackRunning")->Set(m_animator, false);
}

void BossAttackUpperCut::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackUpperCut", false);
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
}

void BossAttackChargeCombo::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackChargeCombo", false);
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
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCharge", false);
}

void BossAttackCharge::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackCharge::OnStateExit()
{
	GetProperty("attackCharge")->Set(m_animator, false);
}

void BossAttackJumpSmashGround::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackJumpSmashGround", false);
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
	GetProperty("attackSpin")->Set(m_animator, false);
}

void BossAttackDoubleUpperCut::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackUpperCut", false);
}

void BossAttackDoubleUpperCut::OnStateUpdate()
{
	if (!m_isChangePose)
	{
		if (GetProperty("currentFrame")->Get<float>(m_animator).Get() > 114)
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
	GetProperty("attackDoubleUpperCut")->Set(m_animator, false);
	m_isChangePose = false;
}
