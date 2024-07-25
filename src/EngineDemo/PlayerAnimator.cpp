#include "PlayerAnimator.h"
#include "SkinnedMesh.h"
#include "Player.h"
#include "PlayerController.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerAnimator)

PlayerAnimator::PlayerAnimator()
	: m_isAnimationEnd(false)
	, m_currentFrame(0)
	, m_currentState(nullptr)
	, m_isWalk(false)
	, m_isRun(false)
	, m_isAttack(false)
	, m_isAttacking(false)
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

	m_animationStateMap["Hit"] = new PlayerHit(this);

	m_currentState = m_animationStateMap["Idle"];
}

void PlayerAnimator::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_playerController = m_owner.lock().get()->GetComponent<PlayerController>().lock();
	m_player = m_owner.lock().get()->GetComponent<Player>().lock();

	m_skinnedMesh->AddAnimation("Idle", L"PlayerAnimations1/Idle/Idle");
	// m_skinnedMesh->AddAnimation("Walk", L"PlayerAnimations1/Move/FrontWalk/Sword And Shield Walk");
	
	Quaternion q = Quaternion::CreateFromYawPitchRoll(Vector3{ 1.5f, 0.0f, 0.0f });
	Matrix t = Matrix::CreateFromQuaternion(q);
	t *= Matrix::CreateTranslation(Vector3{ 0.0f, 0.02f, 0.0f });
	
	m_skinnedMesh->AddAnimation("Walk", L"test2/testWalk2", t);
	m_skinnedMesh->AddAnimation("Run", L"PlayerAnimations1/Move/Run/Sword And Shield Run");
	m_skinnedMesh->AddAnimation("NormalAttack1", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash1");
	m_skinnedMesh->AddAnimation("NormalAttack2", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash2");
	m_skinnedMesh->AddAnimation("NormalAttack3", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash3");
	m_skinnedMesh->AddAnimation("NormalAttack4", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash4");
	/// m_skinnedMesh->AddAnimation("Hit", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash4");

	m_currentState->OnStateEnter();
}

void PlayerAnimator::Update()
{
	//m_skinnedMesh->SetAnimationSpeed(0.1f);
	m_currentFrame = m_skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(m_skinnedMesh.get()).Get();
	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();

	m_currentState->OnStateUpdate();

	if (m_isAttack)
	{
		m_isAttack = false;
	}

	if (GetKeyDown(KEY::LBTN))
	{
		m_isAttack = true;
		return;
	}

	if (m_playerController->GetTypeInfo().GetProperty("forwardInput")->Get<float>(m_playerController.get()).Get() != 0.f
		|| m_playerController->GetTypeInfo().GetProperty("sideInput")->Get<float>(m_playerController.get()).Get() != 0.f)
	{
		if (GetKey(KEY::LSHIFT))
		{
			m_isRun = true;
			m_isWalk = false;
			return;
		}
		else
		{
			m_isRun = false;
		}
		m_isWalk = true;
		return;
	}
	else
	{
		m_isWalk = false;
		m_isRun = false;
		return;
	}
}



void PlayerAnimator::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
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

const Property* AnimationState::GetProperty(const std::string& name)
{
	return m_animator->GetTypeInfo().GetProperty(name.c_str());
}

void PlayerWalk::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Walk", false);
}

void PlayerWalk::OnStateUpdate()
{
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
	GetProperty("isAttacking")->Set(m_animator, true);
}

void NormalAttack1::OnStateUpdate()
{
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 33 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 44))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack2");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void NormalAttack1::OnStateExit()
{
	GetProperty("isAttacking")->Set(m_animator, false);
}

void NormalAttack2::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack2", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void NormalAttack2::OnStateUpdate()
{
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 33 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 41))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack3");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void NormalAttack2::OnStateExit()
{
	GetProperty("isAttacking")->Set(m_animator, false);
}

void NormalAttack3::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack3", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void NormalAttack3::OnStateUpdate()
{
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 29 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 36))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack4");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void NormalAttack3::OnStateExit()
{
	GetProperty("isAttacking")->Set(m_animator, false);
}

void NormalAttack4::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack4", false);
	GetProperty("isAttacking")->Set(m_animator, true);
}

void NormalAttack4::OnStateUpdate()
{
	if (GetProperty("isAttack")->Get<bool>(m_animator).Get() && (GetProperty("currentFrame")->Get<int>(m_animator).Get() > 65 && GetProperty("currentFrame")->Get<int>(m_animator).Get() < 73))
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("NormalAttack1");
		GetProperty("isAttack")->Set(m_animator, false);
	}
	else if (GetProperty("isAnimationEnd")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void NormalAttack4::OnStateExit()
{
	GetProperty("isAttacking")->Set(m_animator, false);
}

void PlayerHit::OnStateEnter()
{
	//dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Hit", true);
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
