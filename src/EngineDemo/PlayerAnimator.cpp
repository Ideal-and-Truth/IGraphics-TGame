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

	m_skinnedMesh->SetAnimation("Idle", false);
	EventBind("PlayerIdle", &PlayerAnimator::PlayerIdle);
	EventBind("PlayerWalk", &PlayerAnimator::PlayerWalk);
	EventBind("PlayerAttack", &PlayerAnimator::PlayerAttack);
}

void PlayerAnimator::Update()
{
	bool isAnimationStart = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationStart")->Get<bool>(m_skinnedMesh.get()).Get();
	bool isAnimationEnd = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationEnd")->Get<bool>(m_skinnedMesh.get()).Get();
	bool isAnimationChanged = m_skinnedMesh->GetTypeInfo().GetProperty("isAnimationChanged")->Get<bool>(m_skinnedMesh.get()).Get();

	if (GetKey(KEY::W) || GetKey(KEY::A) || GetKey(KEY::S) || GetKey(KEY::D))
	{
		m_nextAnimation = "PlayerWalk";
	}
	else if (GetKeyDown(KEY::LBTN))
	{
		m_nextAnimation = "PlayerAttack";
	}
	else if (m_currentAnimation != "PlayerIdle")
	{
		m_nextAnimation = "PlayerIdle";
	}

	if (isAnimationEnd)
	{
		m_currentAnimation = m_nextAnimation;
	}

	EventPublish(m_currentAnimation, nullptr);
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

