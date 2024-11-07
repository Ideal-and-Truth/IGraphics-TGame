#include "BossAnimator.h"
#include "SkinnedMesh.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "PlayerAnimator.h"
#include "SimpleDamager.h"
#include "SoundManager.h"
#include <random>

BOOST_CLASS_EXPORT_IMPLEMENT(BossAnimator)

int BossAnimator::RandomNumber(int _min, int _max)
{
	std::random_device rd;
	// �õ尪���� ���� ���� ���� �ʱ�ȭ
	std::mt19937 gen(rd());
	// �յ� ���� ����
	std::uniform_int_distribution<int> dis(_min, _max);
	// ���� ��ȯ
	return dis(gen);
}

BossAnimator::BossAnimator()
	: m_isPursuit(false)
	, m_strafeMove(false)
	, m_isInRange(false)
	, m_attackSwing(false)
	, m_attackRunning(false)
	, m_attackUpperCut(false)
	, m_attackChargeCombo(false)
	, m_attackSmashGround(false)
	, m_attackCharge(false)
	, m_jumpAttack(false)
	, m_attackSpin(false)
	, m_attackCombo1_1(false)
	, m_attackCombo1_2(false)
	, m_attackCombo1_3(false)
	, m_attackCombo2_1(false)
	, m_attackCombo3_1(false)
	, m_attackSwordShoot(false)
	, m_attackShockWave(false)
	, m_attackTimeSphere(false)
	, m_isDamage(false)
	, m_isDown(false)
	, m_isDodge(false)
	, m_isDeath(false)
	, m_isUseSkill(false)
	, m_isAttacking(false)
	, m_playOnce(false)
	, m_isAnimationEnd(false)
	, m_isSkillActive(false)
	, m_skillCoolTime(false)
	, m_swordShootCoolTime(false)
	, m_shockWaveCoolTime(false)
	, m_flameSwordCoolTime(false)
	, m_lightSpeedDashCoolTime(false)
	, m_timeDistortionCoolTime(false)
	, m_killBoss(false)
	, m_isLockOn(false)
	, m_passingTime(0.f)
	, m_lastHp(0.f)
	, m_hitStopTime(0.f)
	, m_sideMove(0.f)
	, m_attackCount(0)
	, m_downGuage(100.f)
	, m_baseSpeed(0.f)
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
	m_animationStateMap["Entrance"] = new BossEntrance(this);
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
	m_animationStateMap["AttackJump"] = new BossAttackJump(this);
	m_animationStateMap["AttackSpin"] = new BossAttackSpin(this);
	m_animationStateMap["AttackDoubleUpperCut"] = new BossAttackDoubleUpperCut(this);
	m_animationStateMap["AttackCombo1_1"] = new BossAttackCombo1_1(this);
	m_animationStateMap["AttackCombo1_2"] = new BossAttackCombo1_2(this);
	m_animationStateMap["AttackCombo1_3"] = new BossAttackCombo1_3(this);
	m_animationStateMap["AttackCombo2_1"] = new BossAttackCombo2_1(this);
	m_animationStateMap["AttackCombo3_1"] = new BossAttackCombo3_1(this);
	m_animationStateMap["AttackSwordShoot"] = new BossAttackSwordShoot(this);
	m_animationStateMap["AttackShockWave"] = new BossAttackShockWave(this);
	m_animationStateMap["AttackTimeSphere"] = new BossAttackTimeSphere(this);


	m_currentState = m_animationStateMap["Entrance"];
}

void BossAnimator::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_enemyController = m_owner.lock().get()->GetComponent<EnemyController>().lock();
	m_enemy = m_owner.lock().get()->GetComponent<Enemy>().lock();
	auto playerEntity = m_enemy->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.get()).Get().lock();
	m_playerAnimator = playerEntity->GetComponent<PlayerAnimator>().lock();
	m_baseSpeed = m_enemy->GetTypeInfo().GetProperty("speed")->Get<float>(m_enemy.get()).Get();
	auto damager = m_owner.lock()->GetComponent<SimpleDamager>().lock();
	damager->GetTypeInfo().GetProperty("onlyHitOnce")->Set(damager.get(), true);

	m_skinnedMesh->AddAnimation("BossEntranceRoot", L"BossAnimations/Idle/BossEntranceRoot");
	m_skinnedMesh->AddAnimation("BossIdle", L"BossAnimations/Idle/Idle");
	m_skinnedMesh->AddAnimation("BossAttackLightSpeedReady", L"BossAnimations/Attacks/AttackLightSpeedReady");
	m_skinnedMesh->AddAnimation("BossAttackLightSpeedDash", L"BossAnimations/Attacks/AttackLightSpeedDash");
	m_skinnedMesh->AddAnimation("BossAttackChargedCombo", L"BossAnimations/Attacks/AttackChargedCombo");
	m_skinnedMesh->AddAnimation("BossBlackHole", L"BossAnimations/Attacks/BlackHoleSummon");
	m_skinnedMesh->AddAnimation("BossSwordShoot", L"BossAnimations/Attacks/BossSwordShoot");
	m_skinnedMesh->AddAnimation("BossJumpAttack", L"BossAnimations/Attacks/JumpAttack");
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
	m_skinnedMesh->AddAnimation("BossAttackCombo3_1", L"BossAnimations/Attacks/Combo3-1");
	m_skinnedMesh->AddAnimation("BossDodge", L"BossAnimations/Dodge/Dodge");
	m_skinnedMesh->AddAnimation("BossDodgeLeft", L"BossAnimations/Dodge/DodgeLeft");
	m_skinnedMesh->AddAnimation("BossDodgeRight", L"BossAnimations/Dodge/DodgeRight");
	m_skinnedMesh->AddAnimation("BossRun", L"BossAnimations/Run/Run");
	m_skinnedMesh->AddAnimation("BossWalk", L"BossAnimations/Strafe/BossWalk");
	m_skinnedMesh->AddAnimation("BossStrafeL", L"BossAnimations/Strafe/StrafeL");
	m_skinnedMesh->AddAnimation("BossStrafeR", L"BossAnimations/Strafe/StrafeR");
	m_skinnedMesh->AddAnimation("BossDown1", L"BossAnimations/Down/Down1");
	m_skinnedMesh->AddAnimation("BossDown2", L"BossAnimations/Down/Down2");
	m_skinnedMesh->AddAnimation("BossDown3", L"BossAnimations/Down/Down3");

	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Boss\\Boss_Walk_1_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_1_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_2_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_3_Sound.wav", false);
	m_managers.lock()->Sound()->CreateSound(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_4_Sound.wav", false);


	m_currentState->OnStateEnter();

	// m_killBoss = true;
}

void BossAnimator::Update()
{
	if (m_killBoss)
	{
		m_owner.lock()->Destroy();
		// m_managers.lock()->Scene()->ChangeScene("TitleScene");
		return;
	}

	auto playerEntity = m_enemy->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.get()).Get().lock();
	Vector3 playerPos = { playerEntity->GetWorldPosition().x,0.f,playerEntity->GetWorldPosition().z };
	Vector3 bossPos = { m_owner.lock()->GetWorldPosition().x,0.f,m_owner.lock()->GetWorldPosition().z };

	if ((GetKey(KEY::W) || GetKey(KEY::S) || GetKey(KEY::A) || GetKey(KEY::D)) && (playerPos - bossPos).Length() < 15.f
		&& !m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.get()).Get()
		&& m_currentState == m_animationStateMap["Idle"])
	{
		m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Set(m_enemy.get(), true);
	}

	// 	if (GetKeyDown(KEY::_9))
	// 	{
	// 		m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Set(m_enemy.get(), true);
	// 	}
	m_currentFrame = m_skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(m_skinnedMesh.get()).Get();
	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();

	if (m_animationStateMap["Entrance"] == m_currentState)
	{
		if (m_isAnimationEnd)
		{
			ChangeState("Idle");
		}
		m_currentState->OnStateUpdate();
	}

	if (!m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.get()).Get())
	{
		return;
	}


	if (m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get() <= 0.f)
	{
		m_isDeath = true;
		m_isDown = true;
	}

	m_sideMove = m_enemyController->GetTypeInfo().GetProperty("sideMove")->Get<float>(m_enemyController.get()).Get();
	m_isInRange = m_enemyController->GetTypeInfo().GetProperty("isAttackReady")->Get<bool>(m_enemyController.get()).Get();
	m_enemyController->GetTypeInfo().GetProperty("strafeMove")->Set(m_enemyController.get(), m_strafeMove);

	float currentTP = m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get();
	float maxTP = m_enemy->GetTypeInfo().GetProperty("maxTP")->Get<float>(m_enemy.get()).Get();


	// ���Ͽ�
	if (!m_isDown && m_enemy->GetTypeInfo().GetProperty("stunGuage")->Get<float>(m_enemy.get()).Get() > m_downGuage)
	{
		m_isDown = true;
		m_passingTime = 0.f;
	}

	if (m_isDown && m_playOnce)
	{
		//m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 8.f)
		{
			m_enemy->GetTypeInfo().GetProperty("stunGuage")->Set(m_enemy.get(), 0.f);
			m_isDown = false;
			m_passingTime = 0.f;
		}
	}

	// ������ ó��
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

	/// �� ������ �� ����ó��
	if (m_currentPhase == 1)
	{
		Phase1();
	}
	else if (m_currentPhase == 2)
	{
		Phase2();
	}
	else if (m_currentPhase == 3)
	{
		Phase3();
	}

	if (m_isPursuit)
	{
		m_enemy->GetTypeInfo().GetProperty("speed")->Set(m_enemy.get(), m_baseSpeed * 1.5f);
	}
	else
	{
		m_enemy->GetTypeInfo().GetProperty("speed")->Set(m_enemy.get(), m_baseSpeed);
	}

	if (m_isUseSkill || m_isDodge || m_isDown || m_currentState == m_animationStateMap["Down"]
		|| m_currentState == m_animationStateMap["AttackCombo1_1"] || m_currentState == m_animationStateMap["AttackCombo1_2"]
		|| m_currentState == m_animationStateMap["AttackCombo1_3"] || m_currentState == m_animationStateMap["AttackCombo2_1"]
		|| m_currentState == m_animationStateMap["AttackCombo3_1"] || m_currentState == m_animationStateMap["AttackSpin"]
		|| m_currentState == m_animationStateMap["AttackJump"] || m_currentState == m_animationStateMap["AttackRunning"])
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), false);
	}
	else
	{
		m_enemyController->GetTypeInfo().GetProperty("canMove")->Set(m_enemyController.get(), true);
	}


	m_currentState->OnStateUpdate();

}

void BossAnimator::SetImpulse(float power, float pL_mR_M, bool isPassThrough)
{
	m_enemyController->GetTypeInfo().GetProperty("isPassThrough")->Set(m_enemyController.get(), isPassThrough);
	m_enemyController->GetTypeInfo().GetProperty("useImpulse")->Set(m_enemyController.get(), true);
	m_enemyController->GetTypeInfo().GetProperty("impulsePower")->Set(m_enemyController.get(), power);
	m_enemyController->GetTypeInfo().GetProperty("sideImpulse")->Set(m_enemyController.get(), pL_mR_M);
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

void BossAnimator::SetEnemySpeed(float speed)
{
	m_enemy->GetTypeInfo().GetProperty("speed")->Set(m_enemy.get(), speed);
}

void BossAnimator::SetChargeAttack(bool onoff)
{
	auto damager = m_owner.lock()->GetComponent<SimpleDamager>().lock();
	damager->GetTypeInfo().GetProperty("damage")->Set(damager.get(), 3.f);
	damager->GetTypeInfo().GetProperty("onlyHitOnce")->Set(damager.get(), onoff);
}

void BossAnimator::SoundPlay(std::wstring path, bool isDup, int channel)
{
	m_managers.lock()->Sound()->Play(path, isDup, channel);
}

void BossEntrance::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossEntranceRoot", false);
	lastFrame = 0;
}

void BossEntrance::OnStateUpdate()
{
	if ((GetProperty("currentFrame")->Get<int>(m_animator).Get() == 32 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 57
		|| GetProperty("currentFrame")->Get<int>(m_animator).Get() == 79 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 100
		|| GetProperty("currentFrame")->Get<int>(m_animator).Get() == 117 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 139
		|| GetProperty("currentFrame")->Get<int>(m_animator).Get() == 164 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 189
		|| GetProperty("currentFrame")->Get<int>(m_animator).Get() == 214) && lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Boss\\Boss_Walk_1_Sound.wav", true, 52);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}
}

void BossIdle::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossIdle", false);
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isUseSkill")->Set(m_animator, false);
}

void BossIdle::OnStateUpdate()
{
	// 	if (GetProperty("attackCombo1_2")->Get<bool>(m_animator).Get()|| GetProperty("attackCombo1_3")->Get<bool>(m_animator).Get())
	// 	{
	// 		GetProperty("attackCombo1_2")->Set(m_animator, false);
	// 		GetProperty("attackCombo1_3")->Set(m_animator, false);
	// 	}
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
	if (GetProperty("attackCombo1_1")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo1_1");
	}
	if (GetProperty("attackCombo2_1")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo2_1");
	}
	if (GetProperty("attackCombo3_1")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo3_1");
	}
	if (GetProperty("attackSpin")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackSpin");
	}
	if (GetProperty("attackSwordShoot")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackSwordShoot");
	}
	if (GetProperty("attackShockWave")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackShockWave");
	}
	if (GetProperty("attackTimeSphere")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackTimeSphere");
	}
	if (GetProperty("jumpAttack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackJump");
	}
	if (GetProperty("isDodge")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Dodge");
	}
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}

}

void BossRun::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossRun", false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	lastFrame = 0;
}

void BossRun::OnStateUpdate()
{
	if ((GetProperty("currentFrame")->Get<int>(m_animator).Get() == 10 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 24)
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Boss\\Boss_Walk_1_Sound.wav", true, 52);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}

	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}
	if (GetProperty("isInRange")->Get<bool>(m_animator).Get() || GetProperty("passingTime")->Get<float>(m_animator).Get() > 2.f)
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo1_3");
	}
	else
	{
		if (GetProperty("strafeMove")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Strafe");
		}
	}
}

void BossRun::OnStateExit()
{
	GetProperty("isPursuit")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
}

void BossStrafe::OnStateEnter()
{
	m_isChangePose = true;
	lastFrame = 0;
}

void BossStrafe::OnStateUpdate()
{
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}
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
			lastFrame = 0;
		}
		else
		{
			dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossStrafeL", false);
			lastFrame = 0;
		}
	}

	if (m_sidePose > 0.f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 10 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 26)
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Boss\\Boss_Walk_1_Sound.wav", true, 53);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}
	else if (m_sidePose < 0.f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 11 || GetProperty("currentFrame")->Get<int>(m_animator).Get() == 27)
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\09. FootStep_Sound\\Boss\\Boss_Walk_1_Sound.wav", true, 53);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}

	if (GetProperty("passingTime")->Get<float>(m_animator).Get() > 1.f)
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
		//GetProperty("passingTime")->Set(m_animator, 0.f);
	}
}

void BossStrafe::OnStateExit()
{
	GetProperty("strafeMove")->Set(m_animator, false);
}

void BossDodge::OnStateEnter()
{
	isPlay = false;
	dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(false);
}

void BossDodge::OnStateUpdate()
{
	if (!isPlay && GetProperty("attackCount")->Get<int>(m_animator).Get() > 2)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossDodge", false);
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(-250.f, 0.f);
		GetProperty("attackCount")->Set(m_animator, 0);
		isPlay = true;
	}
	else if (!isPlay && GetProperty("sideMove")->Get<float>(m_animator).Get() < 0.f && GetProperty("attackCount")->Get<int>(m_animator).Get() <= 2)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossDodgeLeft", false);
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(250.f, 1.f);
		isPlay = true;
	}
	else if (!isPlay && GetProperty("sideMove")->Get<float>(m_animator).Get() > 0.f && GetProperty("attackCount")->Get<int>(m_animator).Get() <= 2)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossDodgeRight", false);
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(250.f, -1.f);
		isPlay = true;
	}

	if (isPlay && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossDodge::OnStateExit()
{
	isPlay = false;
	GetProperty("isDodge")->Set(m_animator, false);
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
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}
}

void BossAttackSwing::OnStateExit()
{
	GetProperty("attackSwing")->Set(m_animator, false);
}

void BossAttackRunning::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackRun", false);
	dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(false);
	lastFrame = 0;
}

void BossAttackRunning::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 17)
	{
		isReset = false;
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(0.f);
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(700.f, 0.f);
		GetProperty("isLockOn")->Set(m_animator, true);
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 22
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		GetProperty("isAttacking")->Set(m_animator, true);
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_2_Sound.wav", false, 60);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 30)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}

	if (GetProperty("isInRange")->Get<bool>(m_animator).Get() || GetProperty("passingTime")->Get<float>(m_animator).Get() > 1.f)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(1.f);
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}
}

void BossAttackRunning::OnStateExit()
{
	GetProperty("attackRunning")->Set(m_animator, false);
	GetProperty("isLockOn")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
}

void BossAttackUpperCut::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackUpperCut", false);
	GetProperty("isUseSkill")->Set(m_animator, true);
}

void BossAttackUpperCut::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 91)
	{
		GetProperty("isSkillActive")->Set(m_animator, true);
	}
	if (isReset && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}

}

void BossAttackUpperCut::OnStateExit()
{
	GetProperty("attackUpperCut")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("isUseSkill")->Set(m_animator, false);
	GetProperty("flameSwordCoolTime")->Set(m_animator, true);
	isReset = false;
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
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
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
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}
}

void BossAttackSmashGround::OnStateExit()
{
	GetProperty("attackSmashGround")->Set(m_animator, false);
}

void BossAttackCharge::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackLightSpeedReady", false);
	GetProperty("isUseSkill")->Set(m_animator, true);
	dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(false);
	if (GetProperty("currentPhase")->Get<int>(m_animator).Get() > 1)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(0.6f);
	}
}

void BossAttackCharge::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}

	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}

	if (m_changePose && isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 8)
	{
		GetProperty("isPursuit")->Set(m_animator, true);
		GetProperty("strafeMove")->Set(m_animator, false);
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(0.f);
	}

	if (m_changePose && isReset && GetProperty("passingTime")->Get<float>(m_animator).Get() > 1.f)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(1.f);
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(0.f, 0.f, false);
	}

	if (GetProperty("currentPhase")->Get<int>(m_animator).Get() > 1)
	{
		if (!m_changePose && isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 132)
		{
			GetProperty("isSkillActive")->Set(m_animator, true);
		}
	}

	if (!m_changePose && isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 142)
	{
		m_changePose = true;
		isReset = false;
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackLightSpeedDash", false);
		if (GetProperty("currentPhase")->Get<int>(m_animator).Get() > 1)
		{
			dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(700.f, 0.f, true);
		}
		else
		{
			dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(800.f, 0.f, true);
		}
		dynamic_cast<BossAnimator*>(m_animator)->SetChargeAttack(false);
		GetProperty("passingTime")->Set(m_animator, 0.f);
		GetProperty("isLockOn")->Set(m_animator, true);
	}
}

void BossAttackCharge::OnStateExit()
{
	if (GetProperty("currentPhase")->Get<int>(m_animator).Get() > 1)
	{
		GetProperty("lightSpeedDashCoolTime")->Set(m_animator, true);
	}

	dynamic_cast<BossAnimator*>(m_animator)->SetChargeAttack(true);
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(1.f);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("attackCharge")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("isLockOn")->Set(m_animator, false);
	GetProperty("isUseSkill")->Set(m_animator, false);
	m_changePose = false;
	isReset = false;
}

void BossAttackJump::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossJumpAttack", false);
}

void BossAttackJump::OnStateUpdate()
{
	GetProperty("strafeMove")->Set(m_animator, false);
	GetProperty("isPursuit")->Set(m_animator, true);


	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}

	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 113)
	{
		GetProperty("isAttacking")->Set(m_animator, true);
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 118)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}

	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 53 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 115)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(true);
	}

	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 115)
	{
		GetProperty("isLockOn")->Set(m_animator, true);
	}

	if (isReset && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
		isReset = false;
	}
}

void BossAttackJump::OnStateExit()
{
	GetProperty("jumpAttack")->Set(m_animator, false);
	GetProperty("isLockOn")->Set(m_animator, false);
	GetProperty("isPursuit")->Set(m_animator, false);
	isReset = false;
}

void BossAttackSpin::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackSpin", false);
	lastFrame = 0;
}

void BossAttackSpin::OnStateUpdate()
{
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 53)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(250.f, 0.f);
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 62
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		GetProperty("isAttacking")->Set(m_animator, true);
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_4_Sound.wav", false, 61);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 79)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}

	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackSpin::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackSpin")->Set(m_animator, false);
	GetProperty("attackCount")->Set(m_animator, GetProperty("attackCount")->Get<int>(m_animator).Get() + 1);
}

void BossAttackDoubleUpperCut::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackUpperCut2", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void BossAttackDoubleUpperCut::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (!m_isChangePose && isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 91)
	{
		GetProperty("isSkillActive")->Set(m_animator, true);
	}
	if (m_isChangePose && isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 31)
	{
		GetProperty("isSkillActive")->Set(m_animator, true);
	}

	if (!m_isChangePose)
	{
		if (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 114)
		{
			dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackUpperCutMirror", false);
			isReset = false;
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
	isReset = false;
	GetProperty("attackUpperCut")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("flameSwordCoolTime")->Set(m_animator, true);
}

void BossDown::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossDown1", false);
	GetProperty("isLockOn")->Set(m_animator, true);
	GetProperty("passingTime")->Set(m_animator, 0.f);
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
			if (GetProperty("isDeath")->Get<bool>(m_animator).Get())
			{
				GetProperty("killBoss")->Set(m_animator, true);
			}
			else
			{
				dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossDown3", false);
				m_isChangePose = false;
			}
		}
	}
}

void BossDown::OnStateExit()
{
	m_isChangePose = false;
	GetProperty("isLockOn")->Set(m_animator, false);
}

void BossAttackCombo1_1::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo1_1", false);
	lastFrame = 0;
}

void BossAttackCombo1_1::OnStateUpdate()
{
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 52
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(90.f, 0.f);
		GetProperty("isAttacking")->Set(m_animator, true);
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_1_Sound.wav", false, 54);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 66)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}

	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 70)
	{
		if (GetProperty("attackCombo1_2")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo1_2");
		}
		else if (GetProperty("attackCombo1_3")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<BossAnimator*>(m_animator)->ChangeState("AttackCombo1_3");
		}
	}
}

void BossAttackCombo1_1::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackCombo1_1")->Set(m_animator, false);
	isReset = false;
}

void BossAttackCombo1_2::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo1_2", false);
	lastFrame = 0;
}

void BossAttackCombo1_2::OnStateUpdate()
{
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 51
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		GetProperty("isAttacking")->Set(m_animator, true);
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(90.f, 0.f);
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_2_Sound.wav", false, 55);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 79)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}

	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackCombo1_2::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackCombo1_2")->Set(m_animator, false);
	GetProperty("attackCount")->Set(m_animator, GetProperty("attackCount")->Get<int>(m_animator).Get() + 1);
}

void BossAttackCombo1_3::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo1_3", false);
	dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(250.f, 0.f);
	lastFrame = 0;
}

void BossAttackCombo1_3::OnStateUpdate()
{
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("attackCombo1_3")->Get<bool>(m_animator).Get()
		&& GetProperty("currentFrame")->Get<int>(m_animator).Get() < 34)
	{
		GetProperty("isAttacking")->Set(m_animator, true);
		dynamic_cast<BossAnimator*>(m_animator)->SetCanMove(true);
	}

	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 29
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_3_Sound.wav", false, 56);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}

	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 36)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
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
	GetProperty("attackCount")->Set(m_animator, GetProperty("attackCount")->Get<int>(m_animator).Get() + 1);
	isReset = false;
}

void BossAttackCombo2_1::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo2_1", false);
	lastFrame = 0;
}

void BossAttackCombo2_1::OnStateUpdate()
{
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}

	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 34
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(90.f, 0.f);
		GetProperty("isAttacking")->Set(m_animator, true);
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_1_Sound.wav", false, 57);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 63)
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(90.f, 0.f);
		GetProperty("isAttacking")->Set(m_animator, false);
	}


	if (isReset && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackCombo2_1::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackCombo2_1")->Set(m_animator, false);
	GetProperty("attackCount")->Set(m_animator, GetProperty("attackCount")->Get<int>(m_animator).Get() + 1);
}

void BossAttackCombo3_1::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackCombo3_1", false);
	lastFrame = 0;
}

void BossAttackCombo3_1::OnStateUpdate()
{
	if (GetProperty("isDown")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Down");
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		m_isChangePose = true;
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 45
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(90.f, 0.f);
		GetProperty("isAttacking")->Set(m_animator, true);
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_1_Sound.wav", false, 58);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 52)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 82
		&& lastFrame != GetProperty("currentFrame")->Get<int>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetImpulse(90.f, 0.f);
		GetProperty("isAttacking")->Set(m_animator, true);
		dynamic_cast<BossAnimator*>(m_animator)->SoundPlay(L"..\\Resources\\Sounds\\07. Boss_Sound\\Boss_SwingSword_4_Sound.wav", false, 59);
		lastFrame = GetProperty("currentFrame")->Get<int>(m_animator).Get();
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 92)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}

	if (m_isChangePose && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackCombo3_1::OnStateExit()
{
	m_isChangePose = false;
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackCombo3_1")->Set(m_animator, false);
	GetProperty("attackCount")->Set(m_animator, GetProperty("attackCount")->Get<int>(m_animator).Get() + 1);
}

void BossAttackSwordShoot::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossSwordShoot", false);
	GetProperty("isUseSkill")->Set(m_animator, true);
}

void BossAttackSwordShoot::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 29)
	{
		GetProperty("isSkillActive")->Set(m_animator, true);
	}
	if (isReset && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackSwordShoot::OnStateExit()
{
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("attackSwordShoot")->Set(m_animator, false);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("swordShootCoolTime")->Set(m_animator, true);
	GetProperty("isUseSkill")->Set(m_animator, false);
	isReset = false;
}

void BossAttackShockWave::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackShockWave", false);
	GetProperty("isUseSkill")->Set(m_animator, true);
}

void BossAttackShockWave::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
		GetProperty("isLockOn")->Set(m_animator, true);
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 47)
	{
		GetProperty("isSkillActive")->Set(m_animator, true);
	}
	if (isReset && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(0.f);
	}
	if (isReset && !GetProperty("isUseSkill")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimationSpeed(1.f);
	}
}

void BossAttackShockWave::OnStateExit()
{
	GetProperty("attackShockWave")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("shockWaveCoolTime")->Set(m_animator, true);
	GetProperty("isLockOn")->Set(m_animator, false);
	GetProperty("isUseSkill")->Set(m_animator, false);
	isReset = false;
}

void BossAttackTimeSphere::OnStateEnter()
{
	dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossBlackHole", false);
	GetProperty("isUseSkill")->Set(m_animator, true);
}

void BossAttackTimeSphere::OnStateUpdate()
{
	if (!GetProperty("isSkillActive")->Get<bool>(m_animator).Get() && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 14)
	{
		isReset = true;
		GetProperty("isLockOn")->Set(m_animator, true);
		GetProperty("isSkillActive")->Set(m_animator, true);
	}

	if (isReset && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<BossAnimator*>(m_animator)->SetAnimation("BossAttackSmashGround", false);
		m_isChangePose = true;
		isReset = false;
	}

	if (!isReset && m_isChangePose && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 20)
	{
		GetProperty("timeDistortionCoolTime")->Set(m_animator, true);
	}

	if (!isReset && m_isChangePose && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 41)
	{
		dynamic_cast<BossAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void BossAttackTimeSphere::OnStateExit()
{
	GetProperty("isUseSkill")->Set(m_animator, false);
	GetProperty("attackTimeSphere")->Set(m_animator, false);
	GetProperty("passingTime")->Set(m_animator, 0.f);
	GetProperty("isSkillActive")->Set(m_animator, false);
	GetProperty("isLockOn")->Set(m_animator, false);
	isReset = false;
	m_isChangePose = false;
}

void BossAnimator::AllStateReset()
{
	m_isPursuit = false;
	m_strafeMove = false;
	m_attackSwing = false;
	m_attackRunning = false;
	m_attackUpperCut = false;
	m_attackChargeCombo = false;
	m_attackSmashGround = false;
	m_attackCharge = false;
	m_jumpAttack = false;
	m_attackSpin = false;
	m_attackCombo1_1 = false;
	m_attackCombo1_2 = false;
	m_attackCombo1_3 = false;
	m_attackCombo1_3 = false;
	m_attackCombo2_1 = false;
	m_attackSwordShoot = false;
	m_attackShockWave = false;
	m_isDamage = false;
	m_isDown = false;
	m_isDodge = false;
	m_skillCoolTime = false;
	m_isSkillActive = false;
	m_isAttacking = false;
	m_isUseSkill = false;
}

void BossAnimator::Phase1()
{
	int random = RandomNumber(1, 20);

	if (!m_playOnce)
	{
		m_enemy->GetTypeInfo().GetProperty("stunGuage")->Set(m_enemy.get(), 0.f);

		m_attackRunning = true;
		m_playOnce = true;
		m_passingTime = 0.f;
	}
	else
	{
		if (!m_isAttacking && !m_isUseSkill)
		{
			m_passingTime += GetDeltaTime();

			// �����϶�
			if (m_isInRange)
			{
				// ��Ÿ 3��ġ�� �齺��
				if (m_attackCount < 3)
				{
					if (m_currentState == m_animationStateMap["AttackCombo1_1"])
					{
						if (random <= 7)
						{
							m_attackCombo1_2 = true;
							m_attackCombo1_3 = false;
						}
						else if (random > 7 && random <= 13)
						{
							m_attackCombo2_1 = true;
						}
						else
						{
							m_attackSpin = true;
						}
					}

					if (m_passingTime < 1.f)
					{
						m_strafeMove = true;
						m_isPursuit = false;
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
							m_attackCombo3_1 = true;
						}
					}
				}
				// �齺�ܿ�
				else
				{
					m_isDodge = true;
				}
			}
			// ���Ÿ��϶�
			else
			{
				if (m_currentState == m_animationStateMap["AttackCombo1_1"])
				{
					m_attackCombo1_3 = true;
					m_attackCombo1_2 = false;
					m_attackSpin = false;
				}

				if (!m_attackCombo1_3 && m_currentState == m_animationStateMap["Idle"])
				{
					if (random <= 10)
					{
						random = RandomNumber(1, 20);

						if (random <= 10 && !m_swordShootCoolTime)
						{
							m_attackSwordShoot = true;
						}
						else if (random > 10 && !m_shockWaveCoolTime)
						{
							m_attackShockWave = true;
						}
					}
					else
					{
						random = RandomNumber(1, 20);
						if (random <= 12)
						{
							m_strafeMove = false;
							m_isPursuit = true;
						}
						else if (random > 12 && random <= 14)
						{
							m_strafeMove = true;
						}
						else
						{
							m_attackCharge = true;
						}
					}
				}
			}
		}

		if (m_currentState == m_animationStateMap["AttackCharge"])
		{
			m_passingTime += GetDeltaTime();
		}
	}
}

void BossAnimator::Phase2()
{
	int random = RandomNumber(1, 20);

	if (!m_playOnce)
	{
		if (m_currentState != m_animationStateMap["Dodge"] && !m_attackUpperCut && m_currentState != m_animationStateMap["AttackUpperCut"])
		{
			AllStateReset();
			m_isDodge = true;
			ChangeState("Dodge");
			m_enemy->GetTypeInfo().GetProperty("stunGuage")->Set(m_enemy.get(), 0.f);
			m_attackCount = 3;
		}
		if (m_currentState == m_animationStateMap["Dodge"])
		{
			m_skillCoolTime = false;
			m_attackUpperCut = true;
		}
		if (m_currentState == m_animationStateMap["AttackUpperCut"])
		{
			m_playOnce = true;
		}
		m_passingTime = 0.f;
	}
	else
	{
		if (!m_isAttacking && !m_isUseSkill)
		{
			m_passingTime += GetDeltaTime();

			// �����϶�
			if (m_isInRange)
			{
				// ��Ÿ 3��ġ�� �齺��
				if (m_attackCount < 3)
				{
					if (m_currentState == m_animationStateMap["AttackCombo1_1"])
					{
						if (random <= 7)
						{
							m_attackCombo1_2 = true;
							m_attackCombo1_3 = false;
						}
						else if (random > 7 && random <= 13)
						{
							m_attackCombo2_1 = true;
						}
						else
						{
							m_attackSpin = true;
						}
					}

					if (m_passingTime < 1.f)
					{
						m_strafeMove = true;
						m_isPursuit = false;
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
				// �齺�ܿ�
				else
				{
					m_isDodge = true;
				}
			}
			// ���Ÿ��϶�
			else
			{
				if (m_currentState == m_animationStateMap["AttackCombo1_1"])
				{
					m_attackCombo1_3 = true;
					m_attackCombo1_2 = false;
					m_attackSpin = false;
				}

				if (!m_attackCombo1_3 && m_currentState == m_animationStateMap["Idle"])
				{
					if (random <= 14)
					{
						random = RandomNumber(1, 20);

						if (random <= 5 && !m_swordShootCoolTime)
						{
							m_attackSwordShoot = true;
						}
						else if (random > 5 && random <= 10 && !m_shockWaveCoolTime)
						{
							m_attackShockWave = true;
						}
						else if (random > 10 && random <= 15 && !m_timeDistortionCoolTime)
						{
							m_attackTimeSphere = true;
						}
						else if (random > 15 && !m_lightSpeedDashCoolTime)
						{
							m_attackCharge = true;
						}
					}
					else
					{
						random = RandomNumber(1, 20);

						if (random <= 10)
						{
							m_strafeMove = false;
							m_isPursuit = true;
						}
						else if (random > 10 && random <= 16)
						{
							m_attackRunning = true;
						}
						else if (random > 16 && random <= 20)
						{
							m_strafeMove = true;
						}
					}
				}
			}
		}
		if (m_currentState == m_animationStateMap["AttackCharge"] || m_currentState == m_animationStateMap["AttackRunning"])
		{
			m_passingTime += GetDeltaTime();
		}
	}
}

void BossAnimator::Phase3()
{
	float currentTP = m_enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_enemy.get()).Get();
	float maxTP = m_enemy->GetTypeInfo().GetProperty("maxTP")->Get<float>(m_enemy.get()).Get();

	int random = RandomNumber(1, 20);

	if (!m_playOnce)
	{
		if (currentTP / maxTP < 0.5f && m_currentState != m_animationStateMap["Down"] && !m_isDown && !m_jumpAttack)
		{
			AllStateReset();
			m_isDown = true;
			ChangeState("Down");
			m_enemy->GetTypeInfo().GetProperty("stunGuage")->Set(m_enemy.get(), 0.f);
		}
		if (m_currentState == m_animationStateMap["Down"] && m_isDown)
		{
			m_enemy->GetTypeInfo().GetProperty("currentTP")->Set(m_enemy.get(), currentTP + GetDeltaTime() * 5.f);
		}
		if (currentTP / maxTP >= 0.5f && m_currentState == m_animationStateMap["Down"])
		{
			m_enemy->GetTypeInfo().GetProperty("currentTP")->Set(m_enemy.get(), m_enemy->GetTypeInfo().GetProperty("maxTP")->Get<float>(m_enemy.get()).Get() * 0.5f);
			m_enemy->GetTypeInfo().GetProperty("stunGuage")->Set(m_enemy.get(), 0.f);
			m_playOnce = true;
			m_isDown = false;
			m_jumpAttack = true;
		}
	}
	else
	{
		if (!m_isAttacking && !m_isUseSkill)
		{
			m_passingTime += GetDeltaTime();

			// �����϶�
			if (m_isInRange)
			{
				// ��Ÿ 3��ġ�� �齺��
				if (m_attackCount < 3)
				{
					if (m_currentState == m_animationStateMap["AttackCombo1_1"])
					{
						if (random <= 7)
						{
							m_attackCombo1_2 = true;
							m_attackCombo1_3 = false;
						}
						else if (random > 7 && random <= 13)
						{
							m_attackCombo2_1 = true;
						}
						else
						{
							m_attackSpin = true;
						}
					}

					if (m_passingTime < 1.f)
					{
						m_strafeMove = true;
						m_isPursuit = false;
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
				// �齺�ܿ�
				else
				{
					m_isDodge = true;
				}
			}
			// ���Ÿ��϶�
			else
			{
				if (m_currentState == m_animationStateMap["AttackCombo1_1"])
				{
					m_attackCombo1_3 = true;
					m_attackCombo1_2 = false;
					m_attackSpin = false;
				}

				if (!m_attackCombo1_3 && m_currentState == m_animationStateMap["Idle"])
				{
					if (random <= 14)
					{
						random = RandomNumber(1, 20);

						if (random <= 4 && !m_swordShootCoolTime)
						{
							m_attackSwordShoot = true;
						}
						else if (random > 4 && random <= 8 && !m_shockWaveCoolTime)
						{
							m_attackShockWave = true;
						}
						else if (random > 8 && random <= 12 && !m_timeDistortionCoolTime)
						{
							m_attackTimeSphere = true;
						}
						else if (random > 12 && random <= 16 && !m_lightSpeedDashCoolTime)
						{
							m_attackCharge = true;
						}
						else if (random > 16)
						{
							m_jumpAttack = true;
						}
					}
					else
					{
						random = RandomNumber(1, 20);

						if (random <= 10)
						{
							m_strafeMove = false;
							m_isPursuit = true;
						}
						else if (random > 10 && random <= 16)
						{
							m_attackRunning = true;
						}
						else if (random > 16 && random <= 20)
						{
							m_strafeMove = true;
						}
					}
				}
			}
		}
		if (m_currentState == m_animationStateMap["AttackCharge"] || m_currentState == m_animationStateMap["AttackRunning"])
		{
			m_passingTime += GetDeltaTime();
		}
	}
}

