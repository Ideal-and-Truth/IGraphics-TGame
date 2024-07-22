#include "PlayerAnimator.h"
#include "SkinnedMesh.h"
#include "PlayerController.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerAnimator)

PlayerAnimator::PlayerAnimator()
{
	m_name = "PlayerAnimator";
}

PlayerAnimator::~PlayerAnimator()
{

}

void PlayerAnimator::Awake()
{
	m_animationStateMap["Idle"] = new PlayerIdle(this);
	m_animationStateMap["Idle"]->Initialize();

	m_animationStateMap["Walk"] = new PlayerWalk(this);
	m_animationStateMap["Walk"]->Initialize();

	m_animationStateMap["Run"] = new PlayerRun(this);
	m_animationStateMap["Run"]->Initialize();

	m_animationStateMap["NormalAttack1"] = new PlayerAttack(this);
	m_animationStateMap["NormalAttack1"]->Initialize();

	m_currentState = m_animationStateMap["Idle"];
}

void PlayerAnimator::Start()
{
	// 	m_currentAnimation = "PlayerIdle";
	// 	m_nextAnimation = "PlayerIdle";
	// 
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_playerController = m_owner.lock().get()->GetComponent<PlayerController>().lock();

	// 
	m_skinnedMesh->AddAnimation("Idle", L"PlayerAnimations1/Idle/Idle");
	m_skinnedMesh->AddAnimation("Walk", L"PlayerAnimations1/Move/FrontWalk/Sword And Shield Walk");
	m_skinnedMesh->AddAnimation("Run", L"PlayerAnimations1/Move/Run/Sword And Shield Run");
	m_skinnedMesh->AddAnimation("NormalAttack1", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash1");
	m_skinnedMesh->AddAnimation("NormalAttack2", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash2");
	m_skinnedMesh->AddAnimation("NormalAttack3", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash3");
	m_skinnedMesh->AddAnimation("NormalAttack4", L"PlayerAnimations1/NormalAttack/Sword And Shield Slash4");
	//m_skinnedMesh->SetAnimation("Run", true);

	m_currentState->OnStateEnter();
}

void PlayerAnimator::Update()
{
	//m_skinnedMesh->SetAnimationSpeed(0.1f);

	m_currentState->OnStateUpdate();

	if (m_isAnimationChange)
	{
		if (GetKeyDown(KEY::LBTN))
		{
			//m_isAttack1 = true;
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
}



void PlayerAnimator::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
}

void PlayerAnimator::ChangeState(std::string stateName)
{
	m_isAnimationChange = false;
	m_currentState->OnStateExit();
	m_currentState = m_animationStateMap[stateName];
	m_currentState->OnStateEnter();
}

void PlayerIdle::Initialize()
{

}

void PlayerIdle::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Idle", false);
}

void PlayerIdle::OnStateUpdate()
{
	if (GetProperty("isWalk")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Walk");
		GetProperty("isAnimationChange")->Set(m_animator, true);
	}
	if (GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Run");
		GetProperty("isAnimationChange")->Set(m_animator, true);
	}
}

const Property* AnimationState::GetProperty(const std::string& name)
{
	return m_animator->GetTypeInfo().GetProperty(name.c_str());
}

void PlayerWalk::Initialize()
{

}

void PlayerWalk::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Walk", false);
}

void PlayerWalk::OnStateUpdate()
{
	if (GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Run");
		GetProperty("isAnimationChange")->Set(m_animator, true);
	}

	if (!GetProperty("isWalk")->Get<bool>(m_animator).Get() && !GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		GetProperty("isAnimationChange")->Set(m_animator, true);
	}
}

void PlayerRun::Initialize()
{

}

void PlayerRun::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Run", false);
}

void PlayerRun::OnStateUpdate()
{
	if (GetProperty("isWalk")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Walk");
		GetProperty("isAnimationChange")->Set(m_animator, true);
	}

	if (!GetProperty("isWalk")->Get<bool>(m_animator).Get() && !GetProperty("isRun")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<PlayerAnimator*>(m_animator)->ChangeState("Idle");
		GetProperty("isAnimationChange")->Set(m_animator, true);
	}
}

void PlayerAttack::Initialize()
{

}

void PlayerAttack::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("NormalAttack1", true);
}

void PlayerAttack::OnStateUpdate()
{

}
