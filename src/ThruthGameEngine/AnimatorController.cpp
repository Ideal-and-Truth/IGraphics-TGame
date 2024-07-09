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
	

	m_isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();
	if (m_isAnimationEnd)
	{
		CheckTransition();
	}

}

void Truth::AnimatorController::CheckTransition()
{
	
}

void Truth::AnimatorController::NextState()
{

}

