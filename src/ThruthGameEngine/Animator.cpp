#include "Animator.h"
#include "AnimatorController.h"
#include "SkinnedMesh.h"


Truth::Animator::Animator()
{

}

Truth::Animator::~Animator()
{

}

void Truth::Animator::Initalize()
{

}

void Truth::Animator::Awake()
{

}

void Truth::Animator::Start()
{
	m_animatorController = m_owner.lock().get()->GetComponent<Truth::AnimatorController>().lock();
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
}

void Truth::Animator::Update()
{
	m_animatorController->Update();

}

