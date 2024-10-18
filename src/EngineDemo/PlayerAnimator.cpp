#include "PlayerAnimator.h"
#include "Transform.h"
#include "SkinnedMesh.h"
#include "Player.h"
#include "PlayerController.h"
#include "EnemyAnimator.h"
#include "PlayerCamera.h"
#include "Bullet.h"
#include "SimpleDamager.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerAnimator)

PlayerAnimator::PlayerAnimator()
	: m_isAnimationEnd(false)
	, m_currentFrame(0)
	, m_lastHp(0.f)
	, m_hitStopTime(0.f)
	, m_passingTime(0.f)
	, m_currentState(nullptr)
	//, m_isWalk(false)
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
	, m_downAttack(false)
	, m_chargedReady(false)
	, m_normalAttack1(false)
	, m_normalAttack2(false)
	, m_normalAttack3(false)
	, m_normalAttack4(false)
	, m_normalAttack6(false)
	, m_chargedAttack1(false)
	, m_chargedAttack2(false)
	, m_chargedAttack3(false)
	, m_chargedAttack4(false)
	, m_chargedAttack5(false)
	, m_dodgeAttack(false)
	, m_forwardInput(0.f)
	, m_sideInput(0.f)
{
	m_name = "PlayerAnimator";
}

PlayerAnimator::~PlayerAnimator()
{

}

void PlayerAnimator::Awake()
{
	//	m_animationStateMap["Walk"] = new PlayerWalk(this);

	m_animationStateMap["Idle"] = new PlayerIdle(this);

	m_animationStateMap["Run"] = new PlayerRun(this);

	m_animationStateMap["NormalAttack1"] = new NormalAttack1(this);

	m_animationStateMap["NormalAttack2"] = new NormalAttack2(this);

	m_animationStateMap["NormalAttack3"] = new NormalAttack3(this);

	m_animationStateMap["NormalAttack4"] = new NormalAttack4(this);


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

	m_animationStateMap["DodgeAttack"] = new PlayerDodgeAttack(this);

	m_currentState = m_animationStateMap["Idle"];
}

void PlayerAnimator::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_playerController = m_owner.lock().get()->GetComponent<PlayerController>().lock();
	m_player = m_owner.lock().get()->GetComponent<Player>().lock();
	m_playerCamera = m_managers.lock()->Scene()->m_currentScene->FindEntity("ThirdPersonCamera").lock()->GetComponent<PlayerCamera>().lock();

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

	// 걷기 기능 삭제
//	m_skinnedMesh->AddAnimation("Walk", L"PlayerAnimations/Move/FrontWalk/M_katana_Blade@Walk_ver_A_Front");
// 	m_skinnedMesh->AddAnimation("StrafeL", L"PlayerAnimations/Move/FrontWalk/M_katana_Blade@Walk_ver_A_Front_L45");
// 	m_skinnedMesh->AddAnimation("StrafeR", L"PlayerAnimations/Move/FrontWalk/M_katana_Blade@Walk_ver_A_Front_R45");
// 	m_skinnedMesh->AddAnimation("BackStep", L"PlayerAnimations/Move/BackStep/M_katana_Blade@Walk_ver_A_Back");
// 	m_skinnedMesh->AddAnimation("BackStrafeL", L"PlayerAnimations/Move/BackStep/M_katana_Blade@Walk_ver_A_Back_L45");
// 	m_skinnedMesh->AddAnimation("BackStrafeR", L"PlayerAnimations/Move/BackStep/M_katana_Blade@Walk_ver_A_Back_R45");

	m_skinnedMesh->AddAnimation("Idle", L"PlayerAnimations/Idle/Idle");
	m_skinnedMesh->AddAnimation("Run", L"PlayerAnimations/Move/Run");
	m_skinnedMesh->AddAnimation("NormalAttack1", L"PlayerAnimations/NormalAttack/NormalAttack1");
	m_skinnedMesh->AddAnimation("NormalAttack2", L"PlayerAnimations/NormalAttack/NormalAttack2");
	m_skinnedMesh->AddAnimation("NormalAttack3", L"PlayerAnimations/NormalAttack/NormalAttack3");
	m_skinnedMesh->AddAnimation("NormalAttack4", L"PlayerAnimations/NormalAttack/NormalAttack4");
	m_skinnedMesh->AddAnimation("NormalAttack6", L"PlayerAnimations/NormalAttack/NormalAttack6");
	m_skinnedMesh->AddAnimation("NormalVariation1", L"PlayerAnimations/NormalAttack/NormalVariation1");
	m_skinnedMesh->AddAnimation("NormalVariation2", L"PlayerAnimations/NormalAttack/NormalVariation2");
	m_skinnedMesh->AddAnimation("ChargedAttack1", L"PlayerAnimations/ChargedAttack/ChargedAttack1");
	m_skinnedMesh->AddAnimation("ChargedAttack2", L"PlayerAnimations/ChargedAttack/ChargedAttack2");
	m_skinnedMesh->AddAnimation("ChargedAttack3", L"PlayerAnimations/ChargedAttack/ChargedAttack3");
	m_skinnedMesh->AddAnimation("ChargedAttack4", L"PlayerAnimations/ChargedAttack/ChargedAttack4");
	m_skinnedMesh->AddAnimation("ChargedAttack5", L"PlayerAnimations/ChargedAttack/ChargedAttack5");
	m_skinnedMesh->AddAnimation("ChargedAbility", L"PlayerAnimations/ChargedAttack/ChargedAbility");
	m_skinnedMesh->AddAnimation("ComboReady", L"PlayerAnimations/ComboAttackReady/ComboAttackReady");
	m_skinnedMesh->AddAnimation("Guard", L"PlayerAnimations/Guard/GuardLoop");
	m_skinnedMesh->AddAnimation("GuardHit", L"PlayerAnimations/Guard/GuardHit");
	m_skinnedMesh->AddAnimation("Hit", L"PlayerAnimations/Hit/Hit");
	m_skinnedMesh->AddAnimation("Dodge", L"PlayerAnimations/Dodge/Dodge");
	m_skinnedMesh->AddAnimation("DodgeAttack", L"PlayerAnimations/Dodge/DodgeAttack");
	m_skinnedMesh->AddAnimation("SlashSkill", L"PlayerAnimations/Skill/SlashSkill");
	m_skinnedMesh->AddAnimation("TimeStop", L"PlayerAnimations/Skill/TimeStop");


	m_currentState->OnStateEnter();
}

void PlayerAnimator::Update()
{
	if (m_isDead)
	{
		return;
	}

	if (m_player->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_player.get()).Get() < 0.f)
	{
		m_isDead = true;
	}



	m_isLockOn = m_playerCamera->GetTypeInfo().GetProperty("isLockOn")->Get<bool>(m_playerCamera.get()).Get();
	m_forwardInput = m_playerController->GetTypeInfo().GetProperty("forwardInput")->Get<float>(m_playerController.get()).Get();
	m_sideInput = m_playerController->GetTypeInfo().GetProperty("sideInput")->Get<float>(m_playerController.get()).Get();
	m_currentFrame = m_skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(m_skinnedMesh.get()).Get();
	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();


	PlayEffects();

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
			if (GetKey(KEY::W) || GetKey(KEY::A) || GetKey(KEY::S) || GetKey(KEY::D))
			{
				m_isComboReady = false;
			}
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
		m_isRun = false;
		m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);
	}
	else
	{
		m_isGuard = false;
		m_passingTime = 0.f;
	}


	if (m_playerController->GetTypeInfo().GetProperty("canMove")->Get<bool>(m_playerController.get()).Get())
	{
		if (m_playerController->GetTypeInfo().GetProperty("forwardInput")->Get<float>(m_playerController.get()).Get() > 0.f
			|| m_playerController->GetTypeInfo().GetProperty("sideInput")->Get<float>(m_playerController.get()).Get() > 0.f
			|| m_playerController->GetTypeInfo().GetProperty("forwardInput")->Get<float>(m_playerController.get()).Get() < 0.f
			|| m_playerController->GetTypeInfo().GetProperty("sideInput")->Get<float>(m_playerController.get()).Get() < 0.f)
		{
			m_isRun = true;

			if (GetKeyDown(KEY::SPACE))
			{
				m_isDodge = true;
				m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);
			}
		}
		else
		{
			m_isRun = false;
		}
	}

	if (m_chargedReady)
	{
		m_chargedTime += GetDeltaTime();
		if (m_chargedTime > 3.f)
		{
			m_chargedReady = false;
			m_chargedTime = 0.f;
		}
	}

	if (m_currentState == m_animationStateMap["Hit"])
	{
		m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);
	}

	m_currentState->OnStateUpdate();

	if (!m_isDodge && !m_isAttacking && !m_isGuard && !m_isComboReady && !m_isNormalAttack && !m_isChargedAttack && m_currentState != m_animationStateMap["Hit"])
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
		if (enemy->GetComponent<EnemyAnimator>().lock())
		{
			auto enemyAnim = enemy->GetComponent<EnemyAnimator>().lock();
			if (enemyAnim->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(enemyAnim.get()).Get())
			{
				m_isHit = true;
				m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);

				if ((m_passingTime > 0.f && m_passingTime < 0.4f) && enemyAnim->GetTypeInfo().GetProperty("isParryAttack")->Get<bool>(enemyAnim.get()).Get())
				{
					enemyAnim->GetTypeInfo().GetProperty("isDown")->Set(enemyAnim.get(), true);
					m_isHit = false;
				}
				enemyAnim->GetTypeInfo().GetProperty("isAttacking")->Set(enemyAnim.get(), false);
			}
		}
	}

	if (_other->GetOwner().lock()->GetComponent<SimpleDamager>().lock() && !m_isDodge)
	{
		auto damager = _other->GetOwner().lock()->GetComponent<SimpleDamager>().lock();
		if (!damager->GetTypeInfo().GetProperty("onlyHitOnce")->Get<bool>(damager.get()).Get())
		{
			m_isHit = true;
		}
	}

}

void PlayerAnimator::OnCollisionEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->GetComponent<Bullet>().lock() && !m_isDodge)
	{
		m_isHit = true;
		m_playerController->GetTypeInfo().GetProperty("canMove")->Set(m_playerController.get(), false);
	}
}

void PlayerAnimator::SetImpulse(float power, bool needRot)
{
	m_playerController->GetTypeInfo().GetProperty("useImpulse")->Set(m_playerController.get(), true);
	m_playerController->GetTypeInfo().GetProperty("impulsePower")->Set(m_playerController.get(), power);
	m_playerController->GetTypeInfo().GetProperty("needRot")->Set(m_playerController.get(), needRot);
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

	if (GetProperty("chargedReady")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack1");
			GetProperty("isAttack")->Set(m_animator, false);
			GetProperty("chargedReady")->Set(m_animator, false);
		}
	}
	else
	{
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack1");
			GetProperty("isAttack")->Set(m_animator, false);
		}
	}

	if (GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Run");
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

	if (!GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}

	if (GetProperty("chargedReady")->Get<bool>(m_animator).Get())
	{
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack1");
			GetProperty("isAttack")->Set(m_animator, false);
			GetProperty("chargedReady")->Set(m_animator, false);
		}
	}
	else
	{
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack1");
			GetProperty("isAttack")->Set(m_animator, false);
		}
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
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(2.f, true);
}

void NormalAttack1::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Hit");
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 9)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
		isReset = false;
		GetProperty("normalAttack1")->Set(m_animator, true);
	}
	if (GetProperty("isCharged")->Get<float>(m_animator).Get() > 0.3f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 12 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 21))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack6");
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
	GetProperty("isRun")->Set(m_animator, false);
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
}

void NormalAttack2::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack2", false);
	GetProperty("hitStopTime")->Set(m_animator, 0.01f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(2.f, true);
}

void NormalAttack2::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Hit");
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 4)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
		isReset = false;
		GetProperty("normalAttack2")->Set(m_animator, true);
	}
	if (GetProperty("isCharged")->Get<float>(m_animator).Get() > 0.3f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 12 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 36))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack6");
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
	GetProperty("isRun")->Set(m_animator, false);
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
}

void NormalAttack3::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack3", false);
	GetProperty("hitStopTime")->Set(m_animator, 0.f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(2.f, true);
}

void NormalAttack3::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Hit");
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 13)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
		isReset = false;
		GetProperty("normalAttack3")->Set(m_animator, true);
	}
	if (GetProperty("isCharged")->Get<float>(m_animator).Get() > 0.3f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 18 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 37))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack6");
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
	GetProperty("isRun")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
	GetProperty("isAttacking")->Set(m_animator, false);
}

void NormalAttack4::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack4", false);
	GetProperty("hitStopTime")->Set(m_animator, 0.01f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(2.f, true);
}

void NormalAttack4::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Hit");
	}
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 14)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
		isReset = false;
		GetProperty("normalAttack4")->Set(m_animator, true);
	}
	if (GetProperty("isCharged")->Get<float>(m_animator).Get() > 0.3f && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 17 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 41))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack6");
	}

	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
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
	GetProperty("isRun")->Set(m_animator, false);
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
}

void NormalAttack6::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack6", false);
	GetProperty("hitStopTime")->Set(m_animator, 0.f);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isNormalAttack")->Set(m_animator, true);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(3.f, true);
}

void NormalAttack6::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Hit");
	}

	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}

	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 23)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
		GetProperty("downAttack")->Set(m_animator, true);
		isReset = false;
		GetProperty("normalAttack6")->Set(m_animator, true);
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
	GetProperty("isRun")->Set(m_animator, false);
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isNormalAttack")->Set(m_animator, false);
	GetProperty("downAttack")->Set(m_animator, false);
}

void PlayerGuard::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Guard", false);
}

void PlayerGuard::OnStateUpdate()
{
	if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("GuardHit", false);
		isHit = true;
		GetProperty("isHit")->Set(m_animator, false);
	}

	if (isHit && GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Guard", false);
		isHit = false;
	}

	if (!GetProperty("isGuard")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void PlayerGuard::OnStateExit()
{
	GetProperty("isHit")->Set(m_animator, false);
}

void PlayerHit::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Hit", false);
	GetProperty("isHit")->Set(m_animator, false);
}

void PlayerHit::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
	else if (GetProperty("isHit")->Get<bool>(m_animator).Get())
	{
		GetProperty("isHit")->Set(m_animator, false);
		dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Hit", false);
	}
}

void PlayerHit::OnStateExit()
{

}

void ChargedAttack1::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.05f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack1", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isRun")->Set(m_animator, false);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(2.f, true);
	GetProperty("chargedAttack1")->Set(m_animator, true);

}

void ChargedAttack1::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 20)
	{
		isReset = false;
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
	GetProperty("isRun")->Set(m_animator, false);
}

void ChargedAttack2::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.04f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack2", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isRun")->Set(m_animator, false);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(2.f, true);
	GetProperty("chargedAttack2")->Set(m_animator, true);
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
	GetProperty("isRun")->Set(m_animator, false);
}

void ChargedAttack3::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.04f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack3", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isRun")->Set(m_animator, false);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(2.f, true);
}

void ChargedAttack3::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 19)
	{
		GetProperty("chargedAttack3")->Set(m_animator, true);
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 33)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
		isReset = false;
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
	GetProperty("isRun")->Set(m_animator, false);
}

void ChargedAttack4::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.06f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack4", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isRun")->Set(m_animator, false);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(2.f, true);
}

void ChargedAttack4::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 8)
	{
		GetProperty("chargedAttack4")->Set(m_animator, true);
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 25)
	{
		GetProperty("isAttacking")->Set(m_animator, false);
		GetProperty("downAttack")->Set(m_animator, true);
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
	GetProperty("downAttack")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
}

void ChargedAttack5::OnStateEnter()
{
	GetProperty("hitStopTime")->Set(m_animator, 0.07f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ChargedAttack5", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("isChargedAttack")->Set(m_animator, true);
	GetProperty("isRun")->Set(m_animator, false);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(2.f, true);
}

void ChargedAttack5::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() == 0)
	{
		isReset = true;
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() == 19)
	{
		GetProperty("chargedAttack5")->Set(m_animator, true);
	}
	if (isReset && GetProperty("currentFrame")->Get<int>(m_animator).Get() > 24)
	{
		GetProperty("downAttack")->Set(m_animator, true);
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
	GetProperty("downAttack")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
}


void PlayerDodge::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimationSpeed(1.f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(8.f, true);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Dodge", false);
}

void PlayerDodge::OnStateUpdate()
{
	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		GetProperty("isDodge")->Set(m_animator, false);
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
	else if (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 16 && GetProperty("isAttack")->Get<bool>(m_animator).Get())
	{
		GetProperty("isDodge")->Set(m_animator, false);
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("DodgeAttack");
	}
}

void PlayerDodge::OnStateExit()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimationSpeed(1.f);
}

void PlayerDodgeAttack::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("DodgeAttack", false);
	GetProperty("isAttacking")->Set(m_animator, true);
	GetProperty("hitStopTime")->Set(m_animator, 0.07f);
	dynamic_cast<PlayerAnimator*>(m_animator)->SetImpulse(3.f, true);
}

void PlayerDodgeAttack::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get()==13)
	{
		GetProperty("dodgeAttack")->Set(m_animator, true);
	}

	if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void PlayerDodgeAttack::OnStateExit()
{
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
}

void ComboReady::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("ComboReady", false);
	GetProperty("isAttacking")->Set(m_animator, false);
	GetProperty("isRun")->Set(m_animator, false);
	//GetProperty("isWalk")->Set(m_animator, false);
}

void ComboReady::OnStateUpdate()
{
	if (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 25)
	{
		GetProperty("chargedReady")->Set(m_animator, true);
		dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimationSpeed(0.f);
		if (GetProperty("isAttack")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("ChargedAttack1");
			GetProperty("isAttack")->Set(m_animator, false);
			GetProperty("chargedReady")->Set(m_animator, false);
		}
		else if (!GetProperty("isComboReady")->Get<bool>(m_animator).Get())
		{
			dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		}
	}
	else if (!GetProperty("isComboReady")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void ComboReady::OnStateExit()
{
	if (GetProperty("isComboReady")->Get<bool>(m_animator).Get())
	{
		GetProperty("isChargedAttack")->Set(m_animator, true);
		GetProperty("isComboReady")->Set(m_animator, false);
	}
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimationSpeed(1.f);
}

void PlayerAnimator::PlayEffects()
{
	Vector3 effectPos = m_owner.lock()->m_transform->m_position;
	Vector3 effectRot = m_owner.lock()->m_transform->m_rotation.ToEuler();
	effectPos.y += 1.f;

	if (m_normalAttack1)
	{
		m_normalAttack1 = false;

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack.yaml");

			effectRot.z += (3.141592 / 180.f) * 45.f;
			effectRot.x += (3.141592 / 180.f) * 180.f;

			Matrix scaleMT = Matrix::CreateScale(m_owner.lock()->m_transform->m_scale);
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));
			Matrix traslationMT = Matrix::CreateTranslation(effectPos);
			p->SetTransformMatrix(scaleMT * rotationMT * traslationMT);
			p->SetActive(true);
			p->SetSimulationSpeed(3.f);
			p->Play();
		}
	}
	if (m_normalAttack2)
	{
		m_normalAttack2 = false;

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack.yaml");

			Matrix scaleMT = Matrix::CreateScale(m_owner.lock()->m_transform->m_scale);
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));
			Matrix traslationMT = Matrix::CreateTranslation(effectPos);
			p->SetTransformMatrix(scaleMT * rotationMT * traslationMT);
			p->SetActive(true);
			p->SetSimulationSpeed(3.f);
			p->Play();
		}
	}
	if (m_normalAttack3)
	{
		m_normalAttack3 = false;

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack.yaml");

			effectRot.z += (3.141592 / 180.f) * -45.f;
			effectRot.x += (3.141592 / 180.f) * 180.f;

			Matrix scaleMT = Matrix::CreateScale(m_owner.lock()->m_transform->m_scale);
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));
			Matrix traslationMT = Matrix::CreateTranslation(effectPos);
			p->SetTransformMatrix(scaleMT * rotationMT * traslationMT);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}
	if (m_normalAttack4)
	{
		m_normalAttack4 = false;
		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack.yaml");

			effectRot.x += (3.141592 / 180.f) * 180.f;
			effectRot.z += (3.141592 / 180.f) * 45.f;

			Matrix scaleMT = Matrix::CreateScale(m_owner.lock()->m_transform->m_scale);
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));
			Matrix traslationMT = Matrix::CreateTranslation(effectPos);
			p->SetTransformMatrix(scaleMT * rotationMT * traslationMT);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}
	if (m_normalAttack6)
	{
		m_normalAttack6 = false;

		Matrix scaleMT = Matrix::CreateScale(m_owner.lock()->m_transform->m_scale);
		Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));
		Matrix traslationMT = Matrix::CreateTranslation(effectPos);

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack_S.yaml");
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(1.5, 3, 1.5))
				* Matrix::CreateRotationZ(0.13f)
				* traslationMT
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack_S.yaml");
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(1.5, 3, 1.5))
				* Matrix::CreateRotationY(2.44f)
				* Matrix::CreateRotationZ(-0.13f)
				* traslationMT
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\NorAttack_S.yaml");
			p->SetTransformMatrix(
				Matrix::CreateRotationY(2.44f)
				* traslationMT
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}

	if (m_chargedAttack1)
	{
		m_chargedAttack1 = false;

		{
			effectRot.z += (3.141592 / 180.f) * 45.f;
			effectRot.y += (3.141592 / 180.f) * 170.f;
			effectRot.x += (3.141592 / 180.f) * 160.f;

			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack.yaml");
			p->SetTransformMatrix(
				rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}

		{
			effectRot.y += (3.141592 / 180.f) * -20.f;
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack2.yaml");
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(1.5f, 1.2f, 1.5f))
				* rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}

	if (m_chargedAttack2)
	{
		m_chargedAttack2 = false;

		{
			effectRot.z += (3.141592 / 180.f) * 45.f;
			effectRot.y += (3.141592 / 180.f) * 170.f;
			effectRot.x += (3.141592 / 180.f) * 70.f;

			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack.yaml");
			p->SetTransformMatrix(
				rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}

		{
			effectRot.y += (3.141592 / 180.f) * -20.f;
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack2.yaml");
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(1.5f, 1.2f, 1.5f))
				* rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}

	if (m_chargedAttack3)
	{
		m_chargedAttack3 = false;

		{
			effectRot.z += (3.141592 / 180.f) * 90.f;
			effectRot.y += (3.141592 / 180.f) * 20.f;
			effectRot.x += (3.141592 / 180.f) * 20.f;

			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack.yaml");
			p->SetTransformMatrix(
				rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}

		{
			effectRot.y += (3.141592 / 180.f) * -20.f;
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack2.yaml");
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(1.5f, 1.2f, 1.5f))
				* rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}

	if (m_chargedAttack4)
	{
		m_chargedAttack4 = false;

		{
			effectRot.z += (3.141592 / 180.f) * -20.f;
			effectRot.y += (3.141592 / 180.f) * -160.f;
			//effectRot.x += (3.141592 / 180.f) * 70.f;

			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack.yaml");
			p->SetTransformMatrix(
				rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}

		{
			effectRot.y += (3.141592 / 180.f) * -20.f;
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack2.yaml");
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(1.5f, 1.2f, 1.5f))
				* rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}

	if (m_chargedAttack5)
	{
		m_chargedAttack5 = false;

		{
			effectRot.z += (3.141592 / 180.f) * 90.f;
			effectRot.y += (3.141592 / 180.f) * 20.f;
			effectRot.x += (3.141592 / 180.f) * 90.f;

			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack.yaml");
			p->SetTransformMatrix(
				rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}

		{
			effectRot.y += (3.141592 / 180.f) * -20.f;
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\ComAttack2.yaml");
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(1.5f, 1.2f, 1.5f))
				* rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}

	if (m_dodgeAttack)
	{
		m_dodgeAttack = false;

		{
			effectRot.y += (3.141592 / 180.f) * -90.f;
			Matrix rotationMT = Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(effectRot));

			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\DodgeAttack.yaml");
			p->SetDeltaTime(GetDeltaTime());
			p->SetTransformMatrix(
				rotationMT
				* Matrix::CreateTranslation(effectPos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(2.f);
			p->Play();
		}
	}
}
