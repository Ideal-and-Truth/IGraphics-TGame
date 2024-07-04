#include "AnimatorController.h"
#include "SkinnedMesh.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::AnimatorController)

Truth::AnimatorController::AnimatorController()
	: m_currentStateName("")
{
	m_entry.nodeName = "Entry";
	m_entry.animationName = "";
	m_entry.animationSpeed = 0.f;
	m_entry.isDefaultState = false;
	m_entry.isActivated = false;
	m_entry.isLoopTime = false;



	m_exit.nodeName = "Exit";
	m_exit.animationName = "";
	m_exit.animationSpeed = 0.f;
	m_exit.isDefaultState = false;
	m_exit.isActivated = false;
	m_exit.isLoopTime = false;
}

Truth::AnimatorController::~AnimatorController()
{

}

void Truth::AnimatorController::Initalize()
{

}

void Truth::AnimatorController::Awake()
{

}

void Truth::AnimatorController::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
}

void Truth::AnimatorController::Update()
{
	AnimatorState currentState;
	auto components = m_owner.lock().get()->GetTypeInfo().GetProperty("components")->Get<std::vector<std::shared_ptr<Component>>>(m_owner.lock().get()).Get();
	for (auto& c : components)
	{
		if (c.get()->m_name == "States")
		{
			currentState = c.get()->GetTypeInfo().GetProperty("currentState")->Get<AnimatorState>(c.get()).Get();
		}
	}
	m_currentStateName = currentState.nodeName;

	currentState.isActivated = !m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();
	if (currentState.isActivated == false)
	{
		m_skinnedMesh->SetAnimation(currentState.nodeName, currentState.isLoopTime);

	}
	// 	if (m_currentState.nodeName == m_entry.nodeName)
	// 	{
	// 		m_currentState.transitions[0].second = true;
	// 	}
	// 
	// 	for (auto& t : m_currentState.transitions)
	// 	{
	// 		if (t.second == true && m_currentState.isActivated == false)
	// 		{
	// 			m_currentState = t.first;
	// 		}
	// 	}
	// 	if (m_currentState.isActivated == false)
	// 	{
	// 		m_currentState.isActivated = true;
	// 		m_skinnedMesh->SetAnimation(m_currentState.nodeName, m_currentState.isLoopTime);
	// 	}
	// 
	// 	if (isAnimationEnd == true)
	// 	{
	// 		m_currentState.isActivated = false;
	// 	}
}

bool Truth::AnimatorController::IntCondition(std::string comp, int val_1, int val_2)
{
	if (comp == "Greater")
	{
		return (val_1 > val_2) ? true : false;
	}
	else if (comp == "Less")
	{
		return (val_1 < val_2) ? true : false;
	}
	return false;
}

bool Truth::AnimatorController::FloatCondition(std::string comp, float val_1, float val_2)
{
	if (comp == "Greater")
	{
		return (val_1 > val_2) ? true : false;
	}
	else if (comp == "Less")
	{
		return (val_1 < val_2) ? true : false;
	}
	return false;
}

bool Truth::AnimatorController::TriggerCondition(bool val)
{
	if (val)
	{
		val = false;
		return true;
	}
	return false;
}

bool Truth::AnimatorController::BoolCondition(std::string comp, bool val)
{
	if (comp == "true")
	{
		return (val == true) ? true : false;
	}
	else if (comp == "false")
	{
		return (val == false) ? true : false;
	}
	return false;
}

