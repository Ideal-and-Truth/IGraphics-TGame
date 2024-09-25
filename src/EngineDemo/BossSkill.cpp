#include "BossSkill.h"
#include "BossAnimator.h"
#include "Player.h"
#include "BoxCollider.h"
#include "Bullet.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(BossSkill)

BossSkill::BossSkill()
{
	m_name = "BossSkill";
}

BossSkill::~BossSkill()
{

}

void BossSkill::Awake()
{
	m_bossAnimator = m_owner.lock()->GetComponent<BossAnimator>().lock();
	//m_player = .lock()->GetComponent<Player>().lock();
}

void BossSkill::Update()
{

	if (m_bossAnimator->GetTypeInfo().GetProperty("attackSwing")->Get<bool>(m_bossAnimator.get()).Get())
	{
	}
	else if (m_bossAnimator->GetTypeInfo().GetProperty("attackRunning")->Get<bool>(m_bossAnimator.get()).Get())
	{
	}
	else if (m_bossAnimator->GetTypeInfo().GetProperty("attackUpperCut")->Get<bool>(m_bossAnimator.get()).Get())
	{
	}
	else if (m_bossAnimator->GetTypeInfo().GetProperty("attackChargeCombo")->Get<bool>(m_bossAnimator.get()).Get())
	{
	}
	else if (m_bossAnimator->GetTypeInfo().GetProperty("attackSmashGround")->Get<bool>(m_bossAnimator.get()).Get())
	{
	}
	else if (m_bossAnimator->GetTypeInfo().GetProperty("attackCharge")->Get<bool>(m_bossAnimator.get()).Get())
	{
	}
	else if (m_bossAnimator->GetTypeInfo().GetProperty("attackJumpSmashGround")->Get<bool>(m_bossAnimator.get()).Get())
	{
	}
	else if (m_bossAnimator->GetTypeInfo().GetProperty("attackSpin")->Get<bool>(m_bossAnimator.get()).Get())
	{
	}
	else if (m_bossAnimator->GetTypeInfo().GetProperty("attackDoubleUpperCut")->Get<bool>(m_bossAnimator.get()).Get())
	{
	}
}

void BossSkill::ShockWave()
{
	std::shared_ptr<Truth::Entity> shock = std::make_shared<Truth::Entity>(m_managers.lock());
	shock->Initialize();
	shock->m_layer = 1;
	shock->AddComponent<Truth::BoxCollider>(false);

	float damage = 0.f;

	shock->m_name = "ShockWave1";
	m_managers.lock()->Scene()->m_currentScene->CreateEntity(shock);
	shock->SetPosition({ 4.2f,0.f,0.f });
	shock->SetScale({ 300.f,30.f,300.f });

	shock->Awake();
	shock->Start();
}

void BossSkill::FlameSword()
{

}

void BossSkill::SwordShooting()
{

}

void BossSkill::LightSpeedDash(bool isThirdPhase)
{

}

void BossSkill::DistortedTimeSphere()
{

}

void BossSkill::DamageforPlayer()
{

}

