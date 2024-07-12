#include "AnimatorController.h"
#include "SkinnedMesh.h"
#include <memory>

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::AnimatorController)

uint32 Truth::AnimationInfo::m_stateCount = 0;

Truth::AnimatorController::AnimatorController()
{

}

Truth::AnimatorController::~AnimatorController()
{

}

void Truth::AnimatorController::Initalize()
{

}

void Truth::AnimatorController::Awake()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
}

void Truth::AnimatorController::Start()
{
	AddState(std::make_shared<Truth::AnimationInfo>());
	AddState(std::make_shared<Truth::AnimationInfo>());
	AddState(std::make_shared<Truth::AnimationInfo>());

	m_states[0]->m_animationName = "Idle";
	m_states[1]->m_animationName = "Run";
	m_states[2]->m_animationName = "Attack";

	m_states[0]->m_animationPath = L"Kachujin/Idle";
	m_states[1]->m_animationPath = L"Kachujin/Run";
	m_states[2]->m_animationPath = L"Kachujin/Slash";

	m_states[0]->m_isChangesOnFinish = false;
	m_states[1]->m_isChangesOnFinish = false;
	m_states[2]->m_isChangesOnFinish = true;
}

void Truth::AnimatorController::Update()
{


	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();
	if (m_isAnimationEnd)
	{
		CheckTransition();
	}

}

void Truth::AnimatorController::AddState(std::shared_ptr<AnimationInfo> _state)
{
	m_states.push_back(_state);
}

void Truth::AnimatorController::CheckTransition()
{
	for (auto& next : m_currentState->m_nextStates)
	{
		if (0)
		{
			m_currentState = next;
		}
	}
}

void Truth::AnimatorController::NextState()
{
	for (auto& next : m_currentState->m_nextStates)
	{
		if (0)
		{
			m_currentState = next;
		}
	}
	m_skinnedMesh->SetAnimation(m_currentState->m_animationName, m_currentState->m_isChangesOnFinish);

}

Truth::AnimationInfo::AnimationInfo()
	: m_id(m_stateCount++)
{
	m_animationName = "empty";
}

Truth::AnimationInfo::~AnimationInfo()
{

}

void Truth::AnimationInfo::AddTransition(std::shared_ptr<AnimationInfo> _nextState)
{
	m_nextStates.push_back(_nextState);
}

void Truth::AnimationInfo::DeleteTransition(std::shared_ptr<AnimationInfo> _info)
{
	for (auto c = m_nextStates.begin(); c != m_nextStates.end(); c++)
	{
		if ((*c) == _info)
		{
			m_nextStates.erase(c);
			return;
		}
	}
}
