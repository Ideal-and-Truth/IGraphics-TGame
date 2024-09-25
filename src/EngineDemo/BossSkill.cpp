#include "BossSkill.h"
#include "BossAnimator.h"
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
	std::shared_ptr<Truth::Entity> bullet = std::make_shared<Truth::Entity>(m_managers.lock());
	bullet->Initialize();
	bullet->m_layer = 1;
	bullet->AddComponent<Truth::BoxCollider>(false);

	auto bulletComponent = bullet->AddComponent<Bullet>();
	float damage = 0.f;
	bulletComponent->GetTypeInfo().GetProperty("bulletDamage")->Set(bulletComponent.get(), damage);

	bullet->m_name = "bullet";
	m_managers.lock()->Scene()->m_currentScene->CreateEntity(bullet);
	bullet->SetPosition(m_owner.lock()->m_transform->m_worldPosition);
	bullet->SetScale({ 300.f,30.f,300.f });

	bullet->Awake();
	bullet->Start();
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

