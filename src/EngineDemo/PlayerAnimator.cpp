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

	m_animationStateMap["Run"] = new PlayerRun(this);
	m_animationStateMap["Run"]->Initialize();


	m_currentState = m_animationStateMap["Idle"];
}

void PlayerAnimator::Start()
{
	// 	m_currentAnimation = "PlayerIdle";
	// 	m_nextAnimation = "PlayerIdle";
	// 
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	// 
	m_skinnedMesh->AddAnimation("Idle", L"Kachujin/Idle");
	m_skinnedMesh->AddAnimation("Run", L"Kachujin/Run");
	m_skinnedMesh->AddAnimation("Attack", L"Kachujin/Slash");
	//m_skinnedMesh->SetAnimation("Run", true);

	m_currentState->OnStateEnter();
}

void PlayerAnimator::Update()
{
	m_skinnedMesh->SetAnimationSpeed(1.f);

	m_currentState->OnStateUpdate();

	if (m_isAnimationEnd)
	{
		if (!m_isMove)
		{
			ChangeState(m_animationStateMap["Idle"]);
		}
		if (m_isMove)
		{
			ChangeState(m_animationStateMap["Run"]);
		}
	}
}



void PlayerAnimator::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
}

void PlayerAnimator::ChangeState(AnimationState* state)
{
	m_isAnimationEnd = false;
	m_currentState->OnStateExit();
	m_currentState = state;
	m_currentState->OnStateEnter();
}

void PlayerIdle::Initialize()
{
	m_pc = m_owner->GetComponent<PlayerController>().lock().get();
	m_forward = m_pc->GetTypeInfo().GetProperty("forwardInput");
	m_side = m_pc->GetTypeInfo().GetProperty("sideInput");


}

void PlayerIdle::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Idle", false);
}

void PlayerIdle::OnStateUpdate()
{
	if (m_forward->Get<float>(m_pc).Get() != 0.f || m_side->Get<float>(m_pc).Get() != 0.f)
	{
		SetProperty("isMove", true);
		SetProperty("isAnimationEnd", true);
	}

}

const Property* AnimationState::GetProperty(const std::string& name)
{
	return m_animator->GetTypeInfo().GetProperty(name.c_str());
}

void PlayerRun::Initialize()
{
	m_pc = m_owner->GetComponent<PlayerController>().lock().get();
	m_forward = m_pc->GetTypeInfo().GetProperty("forwardInput");
	m_side = m_pc->GetTypeInfo().GetProperty("sideInput");
}

void PlayerRun::OnStateEnter()
{
	dynamic_cast<PlayerAnimator*>(m_animator)->SetAnimation("Run", false);
}

void PlayerRun::OnStateUpdate()
{
	if (m_forward->Get<float>(m_pc).Get() == 0.f && m_side->Get<float>(m_pc).Get() == 0.f)
	{
		SetProperty("isMove", false);
		SetProperty("isAnimationEnd", true);
	}
}
