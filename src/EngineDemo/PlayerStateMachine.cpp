#include "PlayerStateMachine.h"

PlayerStateMachine::PlayerStateMachine()
	: m_isRunning(false)
{
	
}

PlayerStateMachine::~PlayerStateMachine()
{

}

void PlayerStateMachine::Initalize()
{

}

void PlayerStateMachine::Awake()
{

}

void PlayerStateMachine::Start()
{
	m_idle.nodeName = "Idle";
	m_idle.animationName = "Idle";
	m_idle.animationSpeed = 0.f;
	m_idle.isDefaultState = true;
	m_idle.isActivated = false;
	m_idle.isLoopTime = false;

	m_run.nodeName = "Run";
	m_run.animationName = "Run";
	m_run.animationSpeed = 0.f;
	m_run.isDefaultState = false;
	m_run.isActivated = false;
	m_run.isLoopTime = true;

	m_attack.nodeName = "Attack";
	m_attack.animationName = "Attack";
	m_attack.animationSpeed = 0.f;
	m_attack.isDefaultState = false;
	m_attack.isActivated = false;
	m_attack.isLoopTime = false;

	m_currentState = m_entry;
}

void PlayerStateMachine::Update()
{
	//m_isRunning = (m_currentState.nodeName == "Run") ? true : false;
}
