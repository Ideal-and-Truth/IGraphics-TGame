#include "PlayerStateMachine.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerStateMachine)

PlayerStateMachine::PlayerStateMachine()
	: m_currentState("None")
	, m_nextState("None")
{
	m_name = "PlayerStateMachine";
}

PlayerStateMachine::~PlayerStateMachine()
{

}


void PlayerStateMachine::Awake()
{

}

void PlayerStateMachine::Start()
{
	m_currentState = "PlayerIdle";
	m_nextState = "PlayerIdle";
}

void PlayerStateMachine::FixedUpdate()
{

}

void PlayerStateMachine::Update()
{
	EventPublish(m_currentState, nullptr);

	if (GetKey(KEY::W) || GetKey(KEY::A) || GetKey(KEY::S) || GetKey(KEY::D))
	{
		m_currentState = "PlayerWalk";
	}
	else if (GetKey(KEY::LBTN))
	{
		m_currentState = "PlayerAttack";
	}
	else
	{
		m_currentState = "PlayerIdle";
	}
}

void PlayerStateMachine::LateUpdate()
{

}

void PlayerStateMachine::SwitchState(std::string InNextState)
{
	m_nextState = InNextState;
}
