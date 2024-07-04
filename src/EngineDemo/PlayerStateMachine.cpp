#include "PlayerStateMachine.h"
#include "SkinnedMesh.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerStateMachine)


PlayerStateMachine::PlayerStateMachine()
	: m_isRunning(false)
	, m_isAttacking(false)
{
	m_name = "States";
}

PlayerStateMachine::~PlayerStateMachine()
{

}

void PlayerStateMachine::Initalize()
{

}

void PlayerStateMachine::Awake()
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

}

void PlayerStateMachine::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_skinnedMesh->AddAnimation("Idle", L"Kachujin/Idle");
	m_skinnedMesh->AddAnimation("Run", L"Kachujin/Run");
	m_skinnedMesh->AddAnimation("Attack", L"Kachujin/Slash");

}

void PlayerStateMachine::Update()
{
	if (GetKey(KEY::W) || GetKey(KEY::A) || GetKey(KEY::S) || GetKey(KEY::D))
	{
		m_isRunning = true;
	}
	else
	{
		m_isRunning = false;
	}

	if (GetKeyDown(KEY::LBTN))
	{
		m_isAttacking = true;
	}

	if (BoolCondition("true", m_isRunning))
	{
		m_currentState = m_run;
	}

	if (TriggerCondition(m_isAttacking))
	{
		m_currentState = m_attack;
	}
}

