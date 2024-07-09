#include "PlayerAnimator.h"
#include "SkinnedMesh.h"

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

}

void PlayerAnimator::Start()
{
	m_currentAnimation = "PlayerIdle";
	m_nextAnimation = "PlayerIdle";

	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>().lock();

	m_skinnedMesh->AddAnimation("Idle", L"Kachujin/Idle");
	m_skinnedMesh->AddAnimation("Run", L"Kachujin/Run");
	m_skinnedMesh->AddAnimation("Attack", L"Kachujin/Slash");

	m_skinnedMesh->SetAnimation("Run", true);
	EventBind("PlayerIdle", &PlayerAnimator::PlayerIdle);
	EventBind("PlayerWalk", &PlayerAnimator::PlayerWalk);
	EventBind("PlayerAttack", &PlayerAnimator::PlayerAttack);
}

void PlayerAnimator::Update()
{

}

void PlayerAnimator::PlayerIdle(const void*)
{
	m_skinnedMesh->SetAnimation("Idle", false);
}

void PlayerAnimator::PlayerWalk(const void*)
{
	m_skinnedMesh->SetAnimation("Run", false);
}

void PlayerAnimator::PlayerAttack(const void*)
{
	m_skinnedMesh->SetAnimation("Attack", false);
}

