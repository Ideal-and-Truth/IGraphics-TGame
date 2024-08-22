#include "PlayerAnimator.h"
#include "SkinnedMesh.h"
#include "Player.h"
#include "PlayerController.h"
#include "EnemyAnimator.h"
#include "Bullet.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerAnimator)

PlayerAnimator::PlayerAnimator()
	: m_isAnimationEnd(false)
	, m_currentFrame(0)
	, m_lastHp(0.f)
	, m_hitStopTime(0.f)
	, m_passingTime(0.f)
	, m_currentState(nullptr)
	, m_isWalk(false)
	, m_isRun(false)
	, m_isAttack(false)
	, m_isAttacking(false)
	, m_isGuard(false)
	, m_isHit(false)
	, m_isCharged(0.f)
	, m_isChargedAttack(false)
	, m_isDodge(false)
	, m_isLockOn(false)
	, m_isComboReady(false)
	, m_isDead(false)
	, m_isNormalAttack(false)
{
	m_name = "PlayerAnimator";
}

PlayerAnimator::~PlayerAnimator()
{

}

void PlayerAnimator::Awake()
{
	m_animationStateMap["Idle"] = new PlayerIdle(this);

	m_animationStateMap["Walk"] = new PlayerWalk(this);

	m_animationStateMap["Run"] = new PlayerRun(this);

	m_animationStateMap["NormalAttack1"] = new NormalAttack1(this);

	m_animationStateMap["NormalAttack2"] = new NormalAttack2(this);

	m_animationStateMap["NormalAttack3"] = new NormalAttack3(this);

	m_animationStateMap["NormalAttack4"] = new NormalAttack4(this);

	m_animationStateMap["NormalAttack5"] = new NormalAttack5(this);

	m_animationStateMap["NormalAttack6"] = new NormalAttack6(this);

	m_animationStateMap["ComboReady"] = new ComboReady(this);

	m_animationStateMap["ChargedAttack1"] = new ChargedAttack1(this);

	m_animationStateMap["ChargedAttack2"] = new ChargedAttack2(this);

	m_animationStateMap["ChargedAttack3"] = new ChargedAttack3(this);

	m_animationStateMap["ChargedAttack4"] = new ChargedAttack4(this);

	m_animationStateMap["ChargedAttack5"] = new ChargedAttack5(this);

	m_animationStateMap["Guard"] = new PlayerGuard(this);

	m_animationStateMap["Hit"] = new PlayerHit(this);

	m_animationStateMap["Dodge"] = new PlayerDodge(this);

	m_currentState = m_animationStateMap["Idle"];
}

void PlayerAnimator::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_playerController = m_owner.lock().get()->GetComponent<PlayerController>().lock();
	m_player = m_owner.lock().get()->GetComponent<Player>().lock();


	Quaternion q = Quaternion::CreateFromYawPitchRoll(Vector3{ 1.5f, 0.0f, 0.0f });
	Matrix t = Matrix::CreateFromQuaternion(q);
	t *= Matrix::CreateTranslation(Vector3{ 0.0f, 0.02f, 0.0f });

	//m_skinnedMesh->AddAnimation("Walk", L"test2/testWalk2", t);
// 	m_skinnedMesh->AddAnimation("Run", L"PlayerAnimations1/Move/Run/Sword And Shield Run");
// 	m_skinnedMesh->AddAnimation("NormalAttack1", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash1");
// 	m_skinnedMesh->AddAnimation("NormalAttack2", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash2");
// 	m_skinnedMesh->AddAnimation("NormalAttack3", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash3");
// 	m_skinnedMesh->AddAnimation("NormalAttack4", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash4");
// 	m_skinnedMesh->AddAnimation("Guard", L"PlayerAnimations1/Guard/Sword And Shield Block");
// 	m_skinnedMesh->AddAnimation("GuardIdle", L"PlayerAnimations1/Guard/Sword And Shield Block Idle");
// 	m_skinnedMesh->AddAnimation("GuardHit", L"PlayerAnimations1/Guard/Sword And Shield Block Impact");
// 	m_skinnedMesh->AddAnimation("GuardEnd", L"PlayerAnimations1/Guard/Sword And Shield Block End");
// 	m_skinnedMesh->AddAnimation("Hit", L"PlayerAnimations1/Hit/Sword And Shield Impact");
// 	m_skinnedMesh->AddAnimation("Dodge", L"PlayerAnimations1/Dodge/Stand To Roll");

	m_skinnedMesh->AddAnimation("Idle", L"PlayerAnimations/Idle/idle");
	m_skinnedMesh->AddAnimation("Walk", L"PlayerAnimations/Move/FrontWalk/M_katana_Blade@Walk_ver_A_Front");
	m_skinnedMesh->AddAnimation("Run", L"PlayerAnimations/Move/Run/run_strafe_front");
	m_skinnedMesh->AddAnimation("NormalAttack1", L"PlayerAnimations/NormalAttack/Combo_01_1");
	m_skinnedMesh->AddAnimation("NormalAttack2", L"PlayerAnimations/NormalAttack/Combo_01_2");
	m_skinnedMesh->AddAnimation("NormalAttack3", L"PlayerAnimations/NormalAttack/Combo_01_3");
	m_skinnedMesh->AddAnimation("NormalAttack4", L"PlayerAnimations/NormalAttack/Combo_02_3");
	m_skinnedMesh->AddAnimation("NormalAttack5", L"PlayerAnimations/NormalAttack/Combo_03_3");
	m_skinnedMesh->AddAnimation("NormalAttack6", L"PlayerAnimations/NormalAttack/Combo_03_4");
	m_skinnedMesh->AddAnimation("ChargedAttack1", L"PlayerAnimations/ChargedAttack/M_Big_Sword@Attack_3Combo_1");
	m_skinnedMesh->AddAnimation("ChargedAttack2", L"PlayerAnimations/ChargedAttack/M_Big_Sword@Attack_3Combo_2");
	m_skinnedMesh->AddAnimation("ChargedAttack3", L"PlayerAnimations/ChargedAttack/M_Big_Sword@Attack_7Combo_7");
	m_skinnedMesh->AddAnimation("ChargedAttack4", L"PlayerAnimations/ChargedAttack/M_Big_Sword@Attack_3Combo_3");
	m_skinnedMesh->AddAnimation("ChargedAttack5", L"PlayerAnimations/ChargedAttack/M_katana_Blade@Skill_H");
	m_skinnedMesh->AddAnimation("ComboReady", L"PlayerAnimations/ComboAttackReady/Buff_01");
	m_skinnedMesh->AddAnimation("Guard", L"PlayerAnimations/Guard/Revenge_Guard_Loop");
	m_skinnedMesh->AddAnimation("Hit", L"PlayerAnimations/Hit/hit01");
	m_skinnedMesh->AddAnimation("Dodge", L"PlayerAnimations/Dodge/roll_front");

	m_currentState->OnStateEnter();
}

void PlayerAnimator::Update()
{
	if (m_player->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_player.get()).Get() < 0.f)
	{
		m_isDead = true;
	}
	if (m_isDead)
	{
		return;
	}
	//m_skinnedMesh->SetAnimationSpeed(0.1f);
	m_currentFrame = m_skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(m_skinnedMesh.get()).Get();
	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();


	if (m_isAttack)
	{
		m_isAttack = false;
	}

	if (m_isAttacking && !m_isChargedAttack)
	{
		if (GetKey(KEY::W) || GetKey(KEY::A) || GetKey(KEY::S) || GetKey(KEY::D))
		{
			m_isComboReady = false;
		}
		else
		{
			m_isComboReady = true;
		}
	}
	else
	{
		if (m_isComboReady)
		{
			m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);
		}
	}

	if (GetKeyDown(MOUSE::LMOUSE))
	{
		m_isAttack = true;
		m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);
	}

	if (GetKey(MOUSE::LMOUSE))
	{
		m_isCharged += GetDeltaTime();
	}
	else
	{
		m_isCharged = 0.f;
	}

	if (GetKey(MOUSE::RMOUSE))
	{
		m_passingTime += GetDeltaTime();
		m_isGuard = true;
		m_isWalk = false;
		m_isRun = false;
		m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);
	}
	else
	{
		m_isGuard = false;
		m_passingTime = 0.f;
	}

	// 	if (m_lastHp - 5 > m_player->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_player.get()).Get() && m_player->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_player.get()).Get() > 0.f)
	// 	{
	// 		m_isHit = true;
	// 		m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);
	// 	}

	if (m_playerController->GetTypeInfo().GetProperty("canMove")->Get<bool>(m_playerController.get()).Get())
	{
		if (m_playerController->GetTypeInfo().GetProperty("forwardInput")->Get<float>(m_playerController.get()).Get() != 0.f
			|| m_playerController->GetTypeInfo().GetProperty("sideInput")->Get<float>(m_playerController.get()).Get() != 0.f)
		{
			if (GetKey(KEY::LSHIFT))
			{
				m_isRun = true;
				m_isWalk = false;
			}
			else
			{
				m_isRun = false;
				m_isWalk = true;
			}
			if (GetKeyDown(KEY::SPACE))
			{
				m_isDodge = true;
			}
		}
		else
		{
			m_isWalk = false;
			m_isRun = false;
		}
	}

	m_currentState->OnStateUpdate();

	if (!m_isAttacking && !m_isGuard && !m_isHit && !m_isComboReady && !m_isNormalAttack)
	{
		m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), true);
	}



	m_lastHp = m_player->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_player.get()).Get();
}



void PlayerAnimator::OnTriggerEnter(Truth::Collider* _other)
{
	// 적인지 보고
	auto enemy = _other->GetOwner().lock()->m_parent.lock();
	if (enemy)
	{
		auto enemyAnim = enemy->GetComponent<EnemyAnimator>().lock();

		if (enemyAnim)
		{
			if (enemyAnim->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(enemyAnim.get()).Get())
			{
				m_isHit = true;
				m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);

				if ((m_passingTime > 0.f && m_passingTime < 0.2f) && enemyAnim->GetTypeInfo().GetProperty("isParryAttack")->Get<bool>(enemyAnim.get()).Get())
				{
					enemyAnim->GetTypeInfo().GetProperty("isParried")->Set(enemyAnim.get(), true);
				}
			}
		}
	}
}

void PlayerAnimator::OnCollisionEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->GetComponent<Bullet>().lock())
	{
		m_isHit = true;
		m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);
	}
}

void PlayerAnimator::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
}

void PlayerAnimator::SetAnimationSpeed(float speed)
{
	m_skinnedMesh->SetAnimationSpeed(speed);
}

void PlayerAnimator::ChangeState(std::string stateName)
{
	m_currentState->OnStateExit();
	m_currentState = m_animationStateMap[stateName];
	m_currentState->OnStateEnter();
}

void PlayerAnimator::SetPlayerDamage(float damage)
{
	m_player->GetTypeInfo().GetProperty("currentDamage")->Set(m_player.get(), damage);
}

void PlayerIdle::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Idle", false);
}

void PlayerIdle::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Hit");
	}

	if (GetProperty("isGuard")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Guard");
	}

	if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack1");
		GetProperty("isAttack")->Set(m_animator, false);
	}

	if (GetProperty("isWalk")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Walk");
	}

	if (GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Run");
	}
}

void PlayerWalk::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Walk", false);
}

void PlayerWalk::OnStateUpdate()
{
	if (GetProperty("isDodge")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Dodge");
	}

	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Hit");
	}

	if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack1");
		GetProperty("isAttack")->Set(m_animator, false);
	}

	if (GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Run");
	}

	if (!GetProperty("isWalk")->Get<bool>(m_animator).Get() && !GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void PlayerRun::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Run", false);
}

void PlayerRun::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Hit");
	}

	if (GetProperty("isDodge")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Dodge");
	}

	if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack1");
		GetProperty("isAttack")->Set(m_animator, false);
	}

	if (GetProperty("isWalk")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Walk");
	}

	if (!GetProperty("isWalk")->Get<bool>(m_animator).Get() && !GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void NormalAttack1::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack1", false);
	/// TODO : 플레이어 데미지 세팅 여기서 (각 공격마다 세팅해줘야 될 수도)
	///dynamic_cast<PlayerAnimator*>(m_animator)->SetPlayerDamage(1.f);
	GetProperty("hitStopTime")->Set(m_animator, 0.1f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
}

void NormalAttack1::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 12)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 12 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 21))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack2");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isComboReady")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ComboReady");
		}
		else
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}
}

void NormalAttack1::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
}

void NormalAttack2::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack2", false);
	GetProperty("hitStopTime")->Set(m_animator, 0.01f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
}

void NormalAttack2::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 12)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 12 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 36))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack3");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isComboReady")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ComboReady");
		}
		else
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}
}

void NormalAttack2::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
}

void NormalAttack3::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack3", false);
	GetProperty("hitStopTime")->Set(m_animator, 0.f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
}

void NormalAttack3::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 18)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 18 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 37))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack4");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isComboReady")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ComboReady");
		}
		else
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}
}

void NormalAttack3::OnStateExit()
{
	isReset = false;
	GetProperty("isNormalAttack")->Set(m_animator, false);
	GetProperty("isAttacking")->Set(m_animator, false);
}

void NormalAttack4::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack4", false);
	GetProperty("hitStopTime")->Set(m_animator, 0.01f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
}

void NormalAttack4::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 17)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 17 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 41))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack5");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isComboReady")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ComboReady");
		}
		else
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}
}

void NormalAttack4::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
}

void NormalAttack5::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack5", false);
	GetProperty("hitStopTime")->Set(m_animator, 0.03f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
}

void NormalAttack5::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 26)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 26 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 37))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack6");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isComboReady")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ComboReady");
		}
		else
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}
}

void NormalAttack5::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
}

void NormalAttack6::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack6", false);
	GetProperty("hitStopTime")->Set(m_animator, 0.f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
}

void NormalAttack6::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 60)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 60 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 71))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack1");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isComboReady")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ComboReady");
		}
		else
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}
}

void NormalAttack6::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
}

void PlayerGuard::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Guard", false);
}

void PlayerGuard::OnStateUpdate()
{
	if (!GetProperty("isGuard")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void PlayerGuard::OnStateExit()
{
	
}

void PlayerHit::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Hit", true);
}

void PlayerHit::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void PlayerHit::OnStateExit()
{
	GetProperty("isHit")->Set(m_animator, false);
}

void ChargedAttack1::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.05f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack1", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isWalk")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
}

void ChargedAttack1::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 20)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isCharged")->Get<float>(m_animator).Get() > 0.3f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 20 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 33))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack5");
	}
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 20 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 33))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack2");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		GetProperty("isChargedAttack")->Set(m_animator, false);
	}
}

void ChargedAttack1::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
}

void ChargedAttack2::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.04f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack2", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isWalk")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
}

void ChargedAttack2::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 15)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isCharged")->Get<float>(m_animator).Get() > 0.3f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 15 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 25))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack5");
	}
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 15 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 25))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack3");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		GetProperty("isChargedAttack")->Set(m_animator, false);
	}
}

void ChargedAttack2::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
}

void ChargedAttack3::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.04f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack3", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isWalk")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
}

void ChargedAttack3::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 33)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isCharged")->Get<float>(m_animator).Get() > 0.3f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 33 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 48))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack5");
	}
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 33 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 48))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack4");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		GetProperty("isChargedAttack")->Set(m_animator, false);
	}
}

void ChargedAttack3::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
}

void ChargedAttack4::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.06f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack4", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isWalk")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
}

void ChargedAttack4::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 25)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isCharged")->Get<float>(m_animator).Get() > 0.3f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 25 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 35))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack5");
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		GetProperty("isChargedAttack")->Set(m_animator, false);
	}
}

void ChargedAttack4::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
}

void ChargedAttack5::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.07f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack5", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isWalk")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
}

void ChargedAttack5::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 24)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
	}
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		GetProperty("isChargedAttack")->Set(m_animator, false);
	}
}

void ChargedAttack5::OnStateExit()
{
	isReset = false;
	GetProperty("isAttacking")->Set(m_animator, false);
}


void PlayerDodge::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimationSpeed(1.f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Dodge", false);
}

void PlayerDodge::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		GetProperty("isDodge")->Set(m_animator, false);
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Walk");
	}
}

void PlayerDodge::OnStateExit()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimationSpeed(1.f);
}

void ComboReady::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ComboReady", false);
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isWalk")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
}

void ComboReady::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 25)
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimationSpeed(0.f);
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack1");
			GetProperty("isAttack")->Set(m_animator, false);
		}
	}
}

void ComboReady::OnStateExit()
{
	GetProperty("isComboReady")->Set(m_animator, false);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimationSpeed(1.f);
}
