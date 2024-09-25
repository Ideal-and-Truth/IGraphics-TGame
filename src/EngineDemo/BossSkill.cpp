#include "BossSkill.h"
#include "BossAnimator.h"
#include "Player.h"
#include "BoxCollider.h"
#include "Bullet.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(BossSkill)

BossSkill::BossSkill()
	: m_createComplete(false)
	, m_paternEnds(false)
	, m_useSkill(false)
	, m_passingTime(0.f)
	, m_count(0)
{
	m_name = "BossSkill";
}

BossSkill::~BossSkill()
{

}

void BossSkill::Awake()
{
	m_bossAnimator = m_owner.lock()->GetComponent<BossAnimator>().lock();
	m_player = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player").lock()->GetComponent<Player>().lock();

	m_shockWavePos.push_back(4.2f);
	m_shockWavePos.push_back(12.2f);
	m_shockWavePos.push_back(20.2f);
	m_shockWavePos.push_back(28.2f);
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

	if (GetKeyDown(KEY::_0) || m_paternEnds)
	{
		m_useSkill = !m_useSkill;
		m_paternEnds = false;
	}
	if (m_useSkill)
	{
		ShockWave();
	}
}

void BossSkill::ShockWave()
{
	if (!m_createComplete)
	{
		for (int i = 0; i < 16; i++)
		{
			std::shared_ptr<Truth::Entity> shock = std::make_shared<Truth::Entity>(m_managers.lock());
			shock->Initialize();
			shock->m_layer = 1;
			shock->AddComponent<Truth::BoxCollider>();

			shock->m_name = "ShockWave";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(shock);
			m_owner.lock()->AddChild(shock);

			shock->SetPosition({ 0.f,0.f,0.f });
			shock->SetScale({ 300.f,30.f,300.f });


			shock->Awake();
			shock->Start();

			m_attackColliders.push_back(shock);
		}
		m_createComplete = true;
	}
	else
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 0.3f)
		{
			if (m_count % 2 == 0)
			{
				m_attackColliders[0]->SetPosition({ m_shockWavePos[m_count],0.f,0.f });
				m_attackColliders[1]->SetPosition({ -m_shockWavePos[m_count],0.f,0.f });
				m_attackColliders[2]->SetPosition({ 0.f,0.f,m_shockWavePos[m_count] });
				m_attackColliders[3]->SetPosition({ 0.f,0.f,-m_shockWavePos[m_count] });
				m_attackColliders[4]->SetPosition({ m_shockWavePos[m_count],0.f,m_shockWavePos[m_count] });
				m_attackColliders[5]->SetPosition({ m_shockWavePos[m_count],0.f,-m_shockWavePos[m_count] });
				m_attackColliders[6]->SetPosition({ -m_shockWavePos[m_count],0.f,m_shockWavePos[m_count] });
				m_attackColliders[7]->SetPosition({ -m_shockWavePos[m_count],0.f,-m_shockWavePos[m_count] });
			}
			else if (m_count % 2 == 1)
			{
				m_attackColliders[8]->SetPosition({ m_shockWavePos[m_count],0.f,0.f });
				m_attackColliders[9]->SetPosition({ -m_shockWavePos[m_count],0.f,0.f });
				m_attackColliders[10]->SetPosition({ 0.f,0.f,m_shockWavePos[m_count] });
				m_attackColliders[11]->SetPosition({ 0.f,0.f,-m_shockWavePos[m_count] });
				m_attackColliders[12]->SetPosition({ m_shockWavePos[m_count],0.f,m_shockWavePos[m_count] });
				m_attackColliders[13]->SetPosition({ m_shockWavePos[m_count],0.f,-m_shockWavePos[m_count] });
				m_attackColliders[14]->SetPosition({ -m_shockWavePos[m_count],0.f,m_shockWavePos[m_count] });
				m_attackColliders[15]->SetPosition({ -m_shockWavePos[m_count],0.f,-m_shockWavePos[m_count] });
			}
			m_count++;
			m_passingTime = 0.f;
			if (m_count >= m_shockWavePos.size())
			{
				m_count = 0;
				m_paternEnds = true;
				m_createComplete = false;

				for (auto& e : m_attackColliders)
				{
					m_owner.lock()->DeleteChild(e);
					m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e);
				}
				m_attackColliders.clear();
			}
		}
	}

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

void BossSkill::DamageforPlayer(float damage)
{
	float playerHp = m_player->GetTypeInfo().GetProperty("currentTp")->Get<float>(m_player.get()).Get();
	m_player->GetTypeInfo().GetProperty("currentTp")->Set(m_player.get(), playerHp - damage);
}

