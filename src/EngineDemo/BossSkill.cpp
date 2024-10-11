#include "BossSkill.h"
#include "BossAnimator.h"
#include "Player.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "Bullet.h"
#include "Transform.h"
#include "SkinnedMesh.h"
#include "SimpleDamager.h"
#include "TimeDistortion.h"
#include "Controller.h"

BOOST_CLASS_EXPORT_IMPLEMENT(BossSkill)

BossSkill::BossSkill()
	: m_createComplete(false)
	, m_paternEnds(false)
	, m_swordShooting(false)
	, m_shockWave(false)
	, m_flameSword(false)
	, m_lightSpeedDash(false)
	, m_timeDistortion(false)
	, m_deleteFire(false)
	, m_deleteSword(false)
	, m_deleteClone(false)
	, m_deleteTimeSphere(false)
	, m_readyToShoot(false)
	, m_passingTime(0.f)
	, m_count(0)
	, m_currentPhase(0)
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
	m_readyToShoot = m_bossAnimator->GetTypeInfo().GetProperty("isSkillActive")->Get<bool>(m_bossAnimator.get()).Get();

	if (!m_bossAnimator->GetTypeInfo().GetProperty("skillCoolTime")->Get<bool>(m_bossAnimator.get()).Get())
	{
		if (m_bossAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_bossAnimator.get()).Get())
		{
			if (m_bossAnimator->GetTypeInfo().GetProperty("attackSwordShoot")->Get<bool>(m_bossAnimator.get()).Get() && !m_deleteSword)
			{
				m_swordShooting = true;
			}
			else if (m_bossAnimator->GetTypeInfo().GetProperty("attackShockWave")->Get<bool>(m_bossAnimator.get()).Get())
			{
				m_shockWave = true;
			}
			else if (m_bossAnimator->GetTypeInfo().GetProperty("attackUpperCut")->Get<bool>(m_bossAnimator.get()).Get() && !m_deleteFire)
			{
				m_flameSword = true;
			}
			else if (m_bossAnimator->GetTypeInfo().GetProperty("attackCharge")->Get<bool>(m_bossAnimator.get()).Get() && !m_deleteClone)
			{
				m_lightSpeedDash = true;
			}
		}
	}
	else
	{
		m_passingTime += GetDeltaTime();
		if (m_passingTime > 10.f)
		{
			m_bossAnimator->GetTypeInfo().GetProperty("skillCoolTime")->Set(m_bossAnimator.get(), false);
			m_passingTime = 0.f;
		}
	}

	if (m_currentPhase != m_bossAnimator->GetTypeInfo().GetProperty("currentPhase")->Get<int>(m_bossAnimator.get()).Get())
	{
		for (auto& e : m_fires)
		{
			m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
		}

		for (auto& e : m_shockWaves)
		{
			m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
		}

		for (auto& e : m_swords)
		{
			m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
		}

		for (auto& e : m_clones)
		{
			m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
		}

		for (auto& e : m_timeSpheres)
		{
			m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
		}
		m_fires.clear();
		m_shockWaves.clear();
		m_swords.clear();
		m_clones.clear();
		m_timeSpheres.clear();

		m_shootingPos.clear();
		m_illusionPos.clear();

		m_passingTime = 0.f;
		m_count = 0;
		m_createComplete = false;
	}

	m_currentPhase = m_bossAnimator->GetTypeInfo().GetProperty("currentPhase")->Get<int>(m_bossAnimator.get()).Get();


	if (m_deleteFire)
	{
		m_flameSword = false;
		m_passingTime += GetDeltaTime();

		if (m_passingTime > 3.f)
		{
			for (auto& e : m_fires)
			{
				m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
			}

			m_fires.clear();

			m_deleteFire = false;

			m_count = 0;
			m_paternEnds = true;
			m_createComplete = false;
		}
	}

	if (m_deleteSword)
	{
		m_swordShooting = false;
		m_passingTime += GetDeltaTime();

		if (m_passingTime > 2.f)
		{
			for (auto& e : m_swords)
			{
				m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
			}

			m_swords.clear();
			m_shootingPos.clear();

			m_deleteSword = false;

			m_count = 0;
			m_paternEnds = true;
			m_createComplete = false;
		}
	}

	if (m_deleteClone)
	{
		m_lightSpeedDash = false;
		m_passingTime += GetDeltaTime();

		if (m_passingTime > 2.f)
		{
			for (auto& e : m_clones)
			{
				m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
			}

			m_clones.clear();
			m_illusionPos.clear();

			m_deleteClone = false;

			m_count = 0;
			m_paternEnds = true;
			m_createComplete = false;
		}
	}

	if (m_deleteTimeSphere)
	{
		m_timeDistortion = false;
		m_passingTime += GetDeltaTime();

		if (m_passingTime > 2.f)
		{
			for (auto& e : m_timeSpheres)
			{
				m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
			}

			m_timeSpheres.clear();
			m_illusionPos.clear();

			m_deleteTimeSphere = false;

			m_count = 0;
			m_paternEnds = true;
			m_createComplete = false;
		}
	}

	if (m_swordShooting)
	{
		SwordShooting();
	}
	if (m_shockWave)
	{
		ShockWave();
	}
	if (m_flameSword)
	{
		FlameSword();
	}
	if (m_lightSpeedDash)
	{
		LightSpeedDash(m_bossAnimator->GetTypeInfo().GetProperty("currentPhase")->Get<int>(m_bossAnimator.get()).Get() > 1);
	}
	if (m_timeDistortion && !m_deleteTimeSphere)
	{
		DistortedTimeSphere();
	}

	// 	if (GetKeyDown(KEY::_0) || m_paternEnds)
	// 	if (GetKeyDown(KEY::_0)
	// 		m_useSkill = !m_useSkill;
	// 		m_paternEnds = false;
	// 	}
	// 	if (GetKeyDown(MOUSE::RMOUSE))
	// 	{
	// 		// 나중에 보스 애니메이터가 스킬 액티브를 넘겨줄거임
	// 		m_readyToShoot = true;
	// 	}
	// 	if (m_useSkill)
	// 	{
	// 		//ShockWave();
	// 		//FlameSword();
	// 		SwordShooting(); //칼 회전 이상함(꼼수로 고쳤지만 짜침) + 아직 삭제 안함
	// 		//LightSpeedDash(false); //아직 안 만듦
	// 		//LightSpeedDash(true); //아직 삭제 안함
	// 		//DistortedTimeSphere(); //미완
	// 	}
// 	if (GetKeyDown(KEY::_0))
// 	{
// 		m_readyToShoot = !m_readyToShoot;
// 	}
// 
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
			shock->AddComponent<SimpleDamager>();

			shock->m_name = "ShockWave";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(shock);
			m_owner.lock()->AddChild(shock);

			shock->SetPosition({ 0.f,0.f,0.f });
			shock->SetScale({ 5.f,1.f,5.f });


			//shock->Awake();
			shock->Start();

			m_shockWaves.push_back(std::make_pair(shock, false));
		}
		m_createComplete = true;
		m_passingTime = 0.5f;
	}
	else
	{
		if (m_readyToShoot)
		{
			m_passingTime += GetDeltaTime();
			if (m_passingTime > 0.3f)
			{
				float bossHeight = m_owner.lock()->m_transform->m_position.y;
				if (m_count % 2 == 0)
				{
					m_shockWaves[0].first->SetPosition({ m_shockWavePos[m_count],bossHeight,0.f });
					m_shockWaves[1].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,0.f });
					m_shockWaves[2].first->SetPosition({ 0.f,bossHeight,m_shockWavePos[m_count] });
					m_shockWaves[3].first->SetPosition({ 0.f,bossHeight,-m_shockWavePos[m_count] });
					m_shockWaves[4].first->SetPosition({ m_shockWavePos[m_count],bossHeight,m_shockWavePos[m_count] });
					m_shockWaves[5].first->SetPosition({ m_shockWavePos[m_count],bossHeight,-m_shockWavePos[m_count] });
					m_shockWaves[6].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,m_shockWavePos[m_count] });
					m_shockWaves[7].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,-m_shockWavePos[m_count] });
				}
				else if (m_count % 2 == 1)
				{
					m_shockWaves[8].first->SetPosition({ m_shockWavePos[m_count],bossHeight,0.f });
					m_shockWaves[9].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,0.f });
					m_shockWaves[10].first->SetPosition({ 0.f,bossHeight,m_shockWavePos[m_count] });
					m_shockWaves[11].first->SetPosition({ 0.f,bossHeight,-m_shockWavePos[m_count] });
					m_shockWaves[12].first->SetPosition({ m_shockWavePos[m_count],bossHeight,m_shockWavePos[m_count] });
					m_shockWaves[13].first->SetPosition({ m_shockWavePos[m_count],bossHeight,-m_shockWavePos[m_count] });
					m_shockWaves[14].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,m_shockWavePos[m_count] });
					m_shockWaves[15].first->SetPosition({ -m_shockWavePos[m_count],bossHeight,-m_shockWavePos[m_count] });
				}
				m_count++;
				m_passingTime = 0.f;
				if (m_count >= m_shockWavePos.size())
				{
					m_count = 0;
					m_paternEnds = true;
					m_createComplete = false;
					m_shockWave = false;

					for (auto& e : m_shockWaves)
					{
						m_owner.lock()->DeleteChild(e.first);

						m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
					}

					m_shockWaves.clear();

					m_bossAnimator->GetTypeInfo().GetProperty("isAttacking")->Set(m_bossAnimator.get(), false);
				}
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
			flame->AddComponent<SimpleDamager>();

			flame->m_name = "Flame";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(flame);
			m_owner.lock()->AddChild(flame);

			flame->SetPosition({ 0.f,0.f,0.f });
			flame->SetScale({ 5.f,1.f,5.f });
			//flame->SetScale({ 300.f,30.f,300.f });

			flame->Start();

			m_fires.push_back(std::make_pair(flame, false));
		}
		m_createComplete = true;
	}
	else
	{
		if (m_readyToShoot)
		{
			float bossHeight = m_owner.lock()->m_transform->m_position.y;
			m_passingTime += GetDeltaTime();
			if (m_passingTime > 0.1f && m_count < m_flamePos.size())
			{
				m_fires[m_count].first->SetPosition({ 0.f,bossHeight,-m_flamePos[m_count] });
				Vector3 worldPos = m_fires[m_count].first->m_transform->m_worldPosition;
				m_owner.lock()->DeleteChild(m_fires[m_count].first);
				m_owner.lock().reset();
				m_fires[m_count].first->m_transform->m_position = worldPos;
				m_count++;
				m_passingTime = 0.f;

			}
			if (m_count >= m_flamePos.size())
			{
				m_deleteFire = true;
				m_flameSword = false;
				m_passingTime = 0.f;
				//m_bossAnimator->GetTypeInfo().GetProperty("isAttacking")->Set(m_bossAnimator.get(), false);
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
			sword->AddComponent<SimpleDamager>();

			sword->m_name = "Sword";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(sword);
			m_owner.lock()->AddChild(sword);

			sword->SetPosition(m_swordPos[m_count]);
			//sword->SetScale({ 30.f,30.f,300.f });
			sword->SetScale({ 0.8f,0.8f,2.f });


			sword->Start();

			m_swords.push_back(std::make_pair(sword, false));

			m_passingTime = 0.f;
			m_count++;
		}
		if (m_count >= m_swordPos.size())
		{
			m_createComplete = true;
			m_count = 0;
			for (auto& e : m_swords)
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
		Vector3 playerPos = m_player->GetOwner().lock()->m_transform->m_position;
		for (int i = 0; i < m_swords.size(); i++)
		{
			if (!m_swords[i].second)
			{
				Vector3 dir = playerPos - m_swords[i].first->m_transform->m_worldPosition + Vector3{ 0.0f, 1.5f, 0.0f };
				dir.Normalize(dir);
				Quaternion lookRot;
				if (playerPos.z - m_swords[i].first->m_transform->m_worldPosition.z > 0.f)
				{
					dir *= -1.f;
				}
				Quaternion::LookRotation(dir, Vector3::Up, lookRot);
				m_swords[i].first->m_transform->m_rotation = Quaternion::Slerp(m_swords[i].first->m_transform->m_rotation, lookRot, 10.f * GetDeltaTime());
			}
			if (m_swords[i].second)
			{
				m_swords[i].first->m_transform->m_position += (m_shootingPos[i] - m_swords[i].first->m_transform->m_position) * GetDeltaTime() * 10.f;
			}
		}

		if (m_readyToShoot)
		{
			m_passingTime += GetDeltaTime();
			if (m_count < m_swordPos.size() && m_passingTime > 1.2f)
			{
				m_swords[m_count].second = true;


				m_shootingPos.push_back(playerPos);
				m_count++;

				m_passingTime = 0.f;
			}
		}

		if (m_swords[m_swords.size() - 1].first->m_transform->m_position.y <= 0.1f)
		{
			m_deleteSword = true;
			m_swordShooting = false;
			m_passingTime = 0.f;
			m_bossAnimator->GetTypeInfo().GetProperty("isAttacking")->Set(m_bossAnimator.get(), false);
		}
	}

}

void BossSkill::LightSpeedDash(bool isSecondPhase)
{
	if (isSecondPhase)
	{
		if (!m_createComplete)
		{
			m_illusionPos.push_back({ 4.2f,0.f,0.f });
			m_illusionPos.push_back({ -4.2f,0.f,0.f });
			for (int i = 0; i < 2; i++)
			{
				std::shared_ptr<Truth::Entity> illusion = std::make_shared<Truth::Entity>(m_managers.lock());
				illusion->Initialize();
				illusion->m_layer = 1;
				//auto boxCollider = illusion->AddComponent<Truth::BoxCollider>();
				auto skinnedMesh = illusion->AddComponent<Truth::SkinnedMesh>();
				auto damage = illusion->AddComponent<SimpleDamager>();
				auto rigid = illusion->AddComponent<Truth::Controller>();
				damage->GetTypeInfo().GetProperty("damage")->Set(damage.get(), 30.f);
				skinnedMesh->SetSkinnedMesh(L"BossAnimations/Idle/Idle");
				skinnedMesh->AddAnimation("AttackLightSpeedReady", L"BossAnimations/Attacks/AttackLightSpeedReady");
				skinnedMesh->AddAnimation("AttackLightSpeedDash", L"BossAnimations/Attacks/AttackLightSpeedDash");
				illusion->m_name = "Illusion";
				m_managers.lock()->Scene()->m_currentScene->CreateEntity(illusion);
				m_owner.lock()->AddChild(illusion);

				illusion->SetPosition(m_illusionPos[i]);
				illusion->SetScale({ 1.f,1.f,1.f });

				m_clones.push_back(std::make_pair(illusion, false));
				skinnedMesh->SetAnimation("AttackLightSpeedReady", false);
			}
			m_createComplete = true;

			for (auto& e : m_clones)
			{
				Vector3 worldPos = e.first->m_transform->m_worldPosition;
				m_owner.lock()->DeleteChild(e.first);
				m_owner.lock().reset();
				e.first->m_transform->m_position = worldPos;
			}
		}
		else
		{
			Vector3 playerPos = m_player->GetOwner().lock()->m_transform->m_position;

			for (int i = 0; i < m_clones.size(); i++)
			{
				if (!m_clones[i].second && !m_deleteClone)
				{
					Vector3 dir = playerPos - m_clones[i].first->m_transform->m_worldPosition;
					dir.y = 0.f;
					dir.Normalize(dir);
					Quaternion lookRot;
					Quaternion::LookRotation(dir, Vector3::Up, lookRot);
					m_clones[i].first->m_transform->m_rotation = Quaternion::Slerp(m_clones[i].first->m_transform->m_rotation, lookRot, 10.f * GetDeltaTime());
				}
			}

			if (m_count < m_clones.size())
			{
				if (m_readyToShoot)
				{
					m_passingTime += GetDeltaTime();

					if (m_passingTime > 0.8f)
					{
						auto skinnedMesh = m_clones[m_count].first->GetComponent<Truth::SkinnedMesh>().lock();

						m_clones[m_count].second = true;
						m_illusionPos[m_count] = playerPos;
						m_passingTime = 0.f;
						skinnedMesh->SetAnimation("AttackLightSpeedDash", false);
						skinnedMesh->SetAnimationSpeed(1.f);

						auto rigid = m_clones[m_count].first->GetComponent<Truth::Controller>().lock();

						Vector3 power(playerPos - m_clones[m_count].first->m_transform->m_position);
						power.y = 0.f;
						power.Normalize();
						power.y = -100.f;
						power *= GetDeltaTime();
						power *= 120.f;

						rigid->AddImpulse(power);

						m_count++;
					}

					if (m_count >= m_clones.size())
					{
						//m_readyToShoot = false;
					}
				}
			}

			for (int i = 0; i < m_clones.size(); i++)
			{
				auto skinnedMesh = m_clones[i].first->GetComponent<Truth::SkinnedMesh>().lock();

				if (!m_clones[i].second && skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(skinnedMesh.get()).Get() > 142)
				{
					skinnedMesh->SetAnimationSpeed(0.f);

					//m_readyToShoot = true;
				}

				if (m_clones[i].second)
				{
					auto rigid = m_clones[i].first->GetComponent<Truth::Controller>().lock();

					Vector3 zero = Vector3::Zero;
					rigid->Move(zero);

					if (skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(skinnedMesh.get()).Get() < 100 && skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(skinnedMesh.get()).Get() > 8)
					{
						m_clones[i].second = false;
						m_deleteClone = true;
						m_lightSpeedDash = false;
						skinnedMesh->SetAnimationSpeed(0.f);
					}
				}
			}
		}
	}
}

void BossSkill::DistortedTimeSphere()
{
	if (!m_createComplete)
	{
		std::shared_ptr<Truth::Entity> timeSphere = std::make_shared<Truth::Entity>(m_managers.lock());
		timeSphere->Initialize();
		timeSphere->m_layer = 1;
		timeSphere->AddComponent<Truth::SphereCollider>();
		timeSphere->AddComponent<TimeDistortion>();
		timeSphere->m_name = "DistortedTimeSphere";
		m_managers.lock()->Scene()->m_currentScene->CreateEntity(timeSphere);

		timeSphere->SetPosition(m_owner.lock()->m_transform->m_position);
		timeSphere->SetScale({ 4.f,4.f,4.f });


		timeSphere->Start();

		m_timeSpheres.push_back(std::make_pair(timeSphere, false));

		m_createComplete = true;
	}

}

void BossSkill::DamageforPlayer(float damage)
{
	float playerHp = m_player->GetTypeInfo().GetProperty("currentTp")->Get<float>(m_player.get()).Get();
	m_player->GetTypeInfo().GetProperty("currentTp")->Set(m_player.get(), playerHp - damage);
}

