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
	, m_readyToShoot(false)
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

	m_flamePos.push_back(4.2f);
	m_flamePos.push_back(12.2f);
	m_flamePos.push_back(20.2f);
	m_flamePos.push_back(28.2f);
	m_flamePos.push_back(36.2f);

	m_swordPos.push_back({ 0.f,3.7f,0.f });
	m_swordPos.push_back({ 1.1f,4.2f,0.f });
	m_swordPos.push_back({ 2.2f,4.7f,0.f });
	m_swordPos.push_back({ -1.1f,4.2f,0.f });
	m_swordPos.push_back({ -2.2f,4.7f,0.f });

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
	if (GetKeyDown(MOUSE::RMOUSE))
	{
		m_readyToShoot = true;
	}
	if (m_useSkill)
	{
		//ShockWave();
		//FlameSword();
		SwordShooting();
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
			shock->SetScale({ 3.f,1.f,3.f });


			//shock->Awake();
			shock->Start();

			m_attackColliders.push_back(std::make_pair(shock, false));
		}
		m_createComplete = true;
	}
	else
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 0.3f)
		{
			float bossHeight = m_owner.lock()->m_transform->m_position.y;
			if (m_count % 2 == 0)
			{
				m_attackColliders[0].first->SetPosition({ m_shockWavePos[m_count],bossHeight,0.f });
				m_attackColliders[1].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,0.f });
				m_attackColliders[2].first->SetPosition({ 0.f,bossHeight,m_shockWavePos[m_count] });
				m_attackColliders[3].first->SetPosition({ 0.f,bossHeight,-m_shockWavePos[m_count] });
				m_attackColliders[4].first->SetPosition({ m_shockWavePos[m_count],bossHeight,m_shockWavePos[m_count] });
				m_attackColliders[5].first->SetPosition({ m_shockWavePos[m_count],bossHeight,-m_shockWavePos[m_count] });
				m_attackColliders[6].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,m_shockWavePos[m_count] });
				m_attackColliders[7].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,-m_shockWavePos[m_count] });
			}
			else if (m_count % 2 == 1)
			{
				m_attackColliders[8].first->SetPosition({ m_shockWavePos[m_count],bossHeight,0.f });
				m_attackColliders[9].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,0.f });
				m_attackColliders[10].first->SetPosition({ 0.f,bossHeight,m_shockWavePos[m_count] });
				m_attackColliders[11].first->SetPosition({ 0.f,bossHeight,-m_shockWavePos[m_count] });
				m_attackColliders[12].first->SetPosition({ m_shockWavePos[m_count],bossHeight,m_shockWavePos[m_count] });
				m_attackColliders[13].first->SetPosition({ m_shockWavePos[m_count],bossHeight,-m_shockWavePos[m_count] });
				m_attackColliders[14].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,m_shockWavePos[m_count] });
				m_attackColliders[15].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,-m_shockWavePos[m_count] });
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
					m_owner.lock()->DeleteChild(e.first);

					m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
				}

				m_attackColliders.clear();
			}
		}
	}

}

void BossSkill::FlameSword()
{
	if (!m_createComplete)
	{
		for (int i = 0; i < 5; i++)
		{
			std::shared_ptr<Truth::Entity> flame = std::make_shared<Truth::Entity>(m_managers.lock());
			flame->Initialize();
			flame->m_layer = 1;
			flame->AddComponent<Truth::BoxCollider>();

			flame->m_name = "Flame";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(flame);
			m_owner.lock()->AddChild(flame);

			flame->SetPosition({ 0.f,0.f,0.f });
			flame->SetScale({ 3.f,1.f,3.f });


			flame->Start();

			m_attackColliders.push_back(std::make_pair(flame, false));
		}
		m_createComplete = true;
	}
	else
	{
		float bossHeight = m_owner.lock()->m_transform->m_position.y;
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 0.1f && m_count < m_flamePos.size())
		{
			m_attackColliders[m_count].first->SetPosition({ 0.f,bossHeight,-m_flamePos[m_count] });
			Vector3 worldPos = m_attackColliders[m_count].first->m_transform->m_worldPosition;
			m_owner.lock()->DeleteChild(m_attackColliders[m_count].first);
			m_owner.lock().reset();
			m_attackColliders[m_count].first->m_transform->m_position = worldPos;
			m_count++;
			m_passingTime = 0.f;

		}
		if (m_count >= m_flamePos.size())
		{
			if (m_passingTime > 3.f)
			{
				m_count = 0;
				m_paternEnds = true;
				m_createComplete = false;
				for (auto& e : m_attackColliders)
				{
					m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
				}
				m_attackColliders.clear();
			}
		}
	}
}

void BossSkill::SwordShooting()
{
	if (!m_createComplete)
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 0.1f && m_count < m_swordPos.size())
		{
			std::shared_ptr<Truth::Entity> sword = std::make_shared<Truth::Entity>(m_managers.lock());
			sword->Initialize();
			sword->m_layer = 1;
			sword->AddComponent<Truth::BoxCollider>();

			sword->m_name = "Sword";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(sword);
			m_owner.lock()->AddChild(sword);

			sword->SetPosition(m_swordPos[m_count]);
			sword->SetScale({ 30.f,30.f,300.f });


			sword->Start();

			m_attackColliders.push_back(std::make_pair(sword, false));

			m_passingTime = 0.f;
			m_count++;
		}
		if (m_count >= m_swordPos.size())
		{
			m_createComplete = true;
			m_count = 0;
			for (auto& e : m_attackColliders)
			{
				Vector3 worldPos = e.first->m_transform->m_worldPosition;
				m_owner.lock()->DeleteChild(e.first);
				m_owner.lock().reset();
				e.first->m_transform->m_position = worldPos;
			}
		}
	}
	else
	{
		Vector3 playerPos = m_player->GetOwner().lock()->m_transform->m_position + Vector3{ 0.0f, 4.5f, 0.0f };
		for (int i = 0; i < m_attackColliders.size(); i++)
		{
			if (m_attackColliders[i].second == false)
			{
				Vector3 dir = playerPos - m_attackColliders[i].first->m_transform->m_position;
				Vector3 angle = playerPos - m_attackColliders[i].first->m_transform->m_position;
				angle.y = 0.f;
				angle.Normalize(angle);
				if (dir.z > 0.f)
				{
					dir.z *= -1.f;
				}
				dir.Normalize(dir);
				Quaternion lookRot;
				Quaternion lookRot2;
				Quaternion::LookRotation(dir, Vector3::Up, lookRot);
				Quaternion::LookRotation(angle, Vector3::Up, lookRot2);
				m_attackColliders[i].first->m_transform->m_rotation = Quaternion::Slerp(m_attackColliders[i].first->m_transform->m_rotation, lookRot, 10.f * GetDeltaTime());
				m_attackColliders[i].first->m_transform->m_rotation.y =  Quaternion::Slerp(m_attackColliders[i].first->m_transform->m_rotation, lookRot2, 10.f * GetDeltaTime()).y;
				//m_attackColliders[i].first->m_transform->m_rotation.z = 0.f;

			}
			if (m_attackColliders[i].second == true)
			{
				m_attackColliders[i].first->m_transform->m_position += (m_swordPos[i] - m_attackColliders[i].first->m_transform->m_position) * GetDeltaTime() * 10.f;
			}
		}

		if (m_readyToShoot)
		{
			m_passingTime += GetDeltaTime();
			if (m_count < m_swordPos.size() && m_passingTime > 0.2f)
			{
				m_attackColliders[m_count].second = true;
				m_swordPos[m_count] = playerPos- Vector3{ 0.0f, 4.5f, 0.0f };
				m_count++;

				//m_paternEnds = true;
				//m_createComplete = false;

				m_passingTime = 0.f;
			}
		}
	}

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

