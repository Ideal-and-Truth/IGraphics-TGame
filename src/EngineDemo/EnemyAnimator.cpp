#include "EnemyAnimator.h"
#include "SkinnedMesh.h"
#include "Enemy.h"
#include "EnemyController.h"

BOOST_CLASS_EXPORT_IMPLEMENT(EnemyAnimator)

EnemyAnimator::EnemyAnimator()
	: m_skinnedMesh(nullptr)
	, m_enemy(nullptr)
	, m_enemyController(nullptr)
	, m_currentState(nullptr)
	, m_currentFrame(0)
	, m_isChase(false)
	, m_isAttackReady(false)
	, m_isAttack(false)
	, m_isHit(false)
	, m_isDead(false)
	, m_isComeBack(false)
	, m_isAnimationEnd(false)
{

}

EnemyAnimator::~EnemyAnimator()
{

}

void EnemyAnimator::Awake()
{
	m_animationStateMap["Idle"] = new EnemyIdle(this);
	m_animationStateMap["Chase"] = new EnemyChase(this);
	m_animationStateMap["Return"] = new EnemyReturn(this);
	m_animationStateMap["AttackReady"] = new EnemyAttackReady(this);
	m_animationStateMap["Attack"] = new EnemyAttack(this);
	m_animationStateMap["Hit"] = new EnemyHit(this);
	m_animationStateMap["Death"] = new EnemyDeath(this);

	m_currentState = m_animationStateMap["Idle"];
}

void EnemyAnimator::Start()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();
	m_enemyController = m_owner.lock().get()->GetComponent<EnemyController>().lock();
	m_enemy = m_owner.lock().get()->GetComponent<Enemy>().lock();

	m_skinnedMesh->AddAnimation("EnemyIdle", L"Kachujin/Idle");
	m_skinnedMesh->AddAnimation("EnemyWalk", L"Kachujin/Sword And Shield Walk");
	m_skinnedMesh->AddAnimation("EnemyLeftWalk", L"Kachujin/Left Strafe Walking");
	m_skinnedMesh->AddAnimation("EnemyRightWalk", L"Kachujin/Right Strafe Walking");
	m_skinnedMesh->AddAnimation("EnemyBackStep", L"Kachujin/Standing Walk Back");
	m_skinnedMesh->AddAnimation("EnemyAttack", L"Kachujin/Sword And Shield Slash");
	m_skinnedMesh->AddAnimation("EnemyHit", L"Kachujin/Sword And Shield Impact");
	m_skinnedMesh->AddAnimation("EnemyDeath", L"Kachujin/Sword And Shield Death");

	m_currentState->OnStateEnter();
}

void EnemyAnimator::Update()
{
	m_isChase = m_enemy->GetTypeInfo().GetProperty("isTargetIn")->Get<bool>(m_enemy.get()).Get();
	m_isComeBack = m_enemyController->GetTypeInfo().GetProperty("isComeBack")->Get<bool>(m_enemyController.get()).Get();
	m_currentState->OnStateUpdate();
}

void EnemyAnimator::SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished)
{
	m_skinnedMesh->SetAnimation(_name, WhenCurrentAnimationFinished);
}

void EnemyAnimator::SetAnimationSpeed(float speed)
{
	m_skinnedMesh->SetAnimationSpeed(speed);
}

void EnemyAnimator::ChangeState(std::string stateName)
{
	m_currentState->OnStateExit();
	m_currentState = m_animationStateMap[stateName];
	m_currentState->OnStateEnter();
}

void EnemyAnimator::SetEnemyDamage(float damage)
{
	m_enemy->GetTypeInfo().GetProperty("currentDamage")->Set(m_enemy.get(), damage);
}

void EnemyIdle::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyIdle", false);
}

void EnemyIdle::OnStateUpdate()
{
	if (GetProperty("isChase")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Chase");
	}
}

void EnemyChase::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyWalk", false);
}

void EnemyChase::OnStateUpdate()
{
	if (!GetProperty("isChase")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Return");
	}
}

void EnemyReturn::OnStateEnter()
{
	dynamic_cast<EnemyAnimator*>(m_animator)->SetAnimation("EnemyWalk", false);
}

void EnemyReturn::OnStateUpdate()
{
	if (!GetProperty("isComeBack")->Get<bool>(m_animator).Get())
	{
		dynamic_cast<EnemyAnimator*>(m_animator)->ChangeState("Idle");
	}
}

void EnemyAttackReady::OnStateEnter()
{

}

void EnemyAttackReady::OnStateUpdate()
{

}

void EnemyAttack::OnStateEnter()
{

}

void EnemyAttack::OnStateUpdate()
{

}

void EnemyHit::OnStateEnter()
{

}

void EnemyHit::OnStateUpdate()
{

}

void EnemyDeath::OnStateEnter()
{

}

void EnemyDeath::OnStateUpdate()
{

}

