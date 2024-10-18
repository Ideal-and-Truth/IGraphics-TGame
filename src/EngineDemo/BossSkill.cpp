#include <random>
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
#include "ParticleManager.h"
#include "IParticleSystem.h"


BOOST_CLASS_EXPORT_IMPLEMENT(BossSkill)


int BossSkill::RandomNumber(int _min, int _max)
{
	std::random_device rd;
	// 시드값으로 난수 생성 엔진 초기화
	std::mt19937 gen(rd());
	// 균등 분포 정의
	std::uniform_int_distribution<int> dis(_min, _max);
	// 난수 반환
	return dis(gen);
}

BossSkill::BossSkill()
	: m_createComplete1(false)
	, m_createComplete2(false)
	, m_createComplete3(false)
	, m_createComplete4(false)
	, m_createComplete5(false)
	, m_paternEnds(false)
	, m_swordShooting(false)
	, m_shockWave(false)
	, m_flameSword(false)
	, m_lightSpeedDash(false)
	, m_timeDistortion(false)
	, m_deleteFire(false)
	, m_deleteSword(false)
	, m_deleteClone(false)
	, m_readyToShoot(false)
	, m_playShock(false)
	, m_spearImpactCount(0)
	, m_swordShootTime(0.f)
	, m_shockWaveTime(0.f)
	, m_flameSwordTime(0.f)
	, m_lightSpeedDashTime(0.f)
	, m_timeDistortionTime(0.f)
	, m_swordShootCoolTime(0.f)
	, m_shockWaveCoolTime(0.f)
	, m_flameSwordCoolTime(0.f)
	, m_lightSpeedDashCoolTime(0.f)
	, m_timeDistortionCoolTime(0.f)
	, m_shockCount(0)
	, m_swordCount(0)
	, m_flameCount(0)
	, m_cloneCount(0)
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
	m_shockWavePos.push_back(8.2f);
	m_shockWavePos.push_back(12.2f);
	m_shockWavePos.push_back(16.2f);

	m_flamePos.push_back(4.2f);
	m_flamePos.push_back(7.2f);
	m_flamePos.push_back(10.2f);
	m_flamePos.push_back(13.2f);
	m_flamePos.push_back(16.2f);

	m_swordPos.push_back({ 0.f,3.7f,0.f });
	m_swordPos.push_back({ 1.1f,4.2f,0.f });
	m_swordPos.push_back({ 2.2f,4.7f,0.f });
	m_swordPos.push_back({ -1.1f,4.2f,0.f });
	m_swordPos.push_back({ -2.2f,4.7f,0.f });

	m_spherePos.push_back({ 9.f, 7.f, 0.f });
	m_spherePos.push_back({ -9.f, 7.f, 0.f });
	m_spherePos.push_back({ 0.f, 7.f, 9.f });
	m_spherePos.push_back({ 0.f, 7.f, -9.f });
	m_spherePos.push_back({ sqrt(40.5f), 7.f, sqrt(40.5f) });
	m_spherePos.push_back({ sqrt(40.5f), 7.f, -sqrt(40.5f) });
	m_spherePos.push_back({ -sqrt(40.5f), 7.f, sqrt(40.5f) });
	m_spherePos.push_back({ -sqrt(40.5f), 7.f, -sqrt(40.5f) });



}

void BossSkill::Update()
{
	/// 작업중 ////////////////////////////////////////////////////////////////////

// 	if (GetKeyDown(KEY::_8))
// 	{
// 		m_readyToShoot = true;
// 	}
// 	DistortedTimeSphere();
// 	DeleteCheck();
// 	return;
	/// 작업중 ////////////////////////////////////////////////////////////////////

	m_readyToShoot = m_bossAnimator->GetTypeInfo().GetProperty("isSkillActive")->Get<bool>(m_bossAnimator.get()).Get();

	if (m_bossAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_bossAnimator.get()).Get())
	{
		if (m_bossAnimator->GetTypeInfo().GetProperty("attackSwordShoot")->Get<bool>(m_bossAnimator.get()).Get() && !m_deleteSword)
		{
			m_swordShooting = true;
			m_bossAnimator->GetTypeInfo().GetProperty("attackSwordShoot")->Set(m_bossAnimator.get(), false);
		}
		else if (m_bossAnimator->GetTypeInfo().GetProperty("attackShockWave")->Get<bool>(m_bossAnimator.get()).Get())
		{
			m_shockWave = true;
			m_bossAnimator->GetTypeInfo().GetProperty("attackShockWave")->Set(m_bossAnimator.get(), false);
		}
		else if (m_bossAnimator->GetTypeInfo().GetProperty("attackUpperCut")->Get<bool>(m_bossAnimator.get()).Get() && !m_deleteFire)
		{
			m_flameSword = true;
			m_bossAnimator->GetTypeInfo().GetProperty("attackUpperCut")->Set(m_bossAnimator.get(), false);
		}
		else if (m_bossAnimator->GetTypeInfo().GetProperty("attackCharge")->Get<bool>(m_bossAnimator.get()).Get() && !m_deleteClone)
		{
			m_lightSpeedDash = true;
			m_bossAnimator->GetTypeInfo().GetProperty("attackCharge")->Set(m_bossAnimator.get(), false);
		}
		else if (m_bossAnimator->GetTypeInfo().GetProperty("attackTimeSphere")->Get<bool>(m_bossAnimator.get()).Get())
		{
			m_timeDistortion = true;
			m_bossAnimator->GetTypeInfo().GetProperty("attackTimeSphere")->Set(m_bossAnimator.get(), false);
		}
	}

	DeleteCheck();

	m_currentPhase = m_bossAnimator->GetTypeInfo().GetProperty("currentPhase")->Get<int>(m_bossAnimator.get()).Get();

	CoolTimeCheck();

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
		LightSpeedDash(m_currentPhase > 1);
	}
	if (m_timeDistortion)
	{
		DistortedTimeSphere();
	}



}

void BossSkill::ShockWave()
{
	if (!m_createComplete1)
	{
		for (int i = 0; i < 16; i++)
		{
			std::shared_ptr<Truth::Entity> shock = std::make_shared<Truth::Entity>(m_managers.lock());
			shock->Initialize();
			shock->m_layer = 1;
			shock->AddComponent<Truth::SphereCollider>();
			auto damage = shock->AddComponent<SimpleDamager>();
			damage->GetTypeInfo().GetProperty("damage")->Set(damage.get(), 3.f);

			shock->m_name = "ShockWave";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(shock);
			m_owner.lock()->AddChild(shock);

			shock->SetPosition({ 0.f,0.f,0.f });
			shock->SetScale({ 5.f,3.f,5.f });


			//shock->Awake();
			shock->Start();

			m_shockWaves.push_back(std::make_pair(shock, false));
		}
		m_createComplete1 = true;
		m_shockWaveTime = 0.5f;
	}
	else
	{
		if (m_readyToShoot)
		{
			m_shockWaveTime += GetDeltaTime();

			if (m_shockWaveTime > 0.3f)
			{
				float bossHeight = m_owner.lock()->m_transform->m_position.y;

				auto pos = sqrt(pow(m_shockWavePos[m_shockCount], 2.f) / 2.f);

				if (m_shockCount % 2 == 0)
				{
					m_shockWaves[0].first->SetPosition({ m_shockWavePos[m_shockCount],bossHeight,0.f });
					m_shockWaves[1].first->SetPosition({ -m_shockWavePos[m_shockCount],bossHeight,0.f });
					m_shockWaves[2].first->SetPosition({ 0.f,bossHeight,m_shockWavePos[m_shockCount] });
					m_shockWaves[3].first->SetPosition({ 0.f,bossHeight,-m_shockWavePos[m_shockCount] });
					m_shockWaves[4].first->SetPosition({ pos,bossHeight,pos });
					m_shockWaves[5].first->SetPosition({ pos,bossHeight,-pos });
					m_shockWaves[6].first->SetPosition({ -pos,bossHeight,pos });
					m_shockWaves[7].first->SetPosition({ -pos,bossHeight,-pos });

					m_playShock = true;
					PlayEffect(m_shockWaves[0].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[1].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[2].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[3].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[4].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[5].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[6].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[7].first->GetWorldPosition());
				}
				else if (m_shockCount % 2 == 1)
				{
					m_shockWaves[8].first->SetPosition({ m_shockWavePos[m_shockCount],bossHeight,0.f });
					m_shockWaves[9].first->SetPosition({ -m_shockWavePos[m_shockCount],bossHeight,0.f });
					m_shockWaves[10].first->SetPosition({ 0.f,bossHeight,m_shockWavePos[m_shockCount] });
					m_shockWaves[11].first->SetPosition({ 0.f,bossHeight,-m_shockWavePos[m_shockCount] });
					m_shockWaves[12].first->SetPosition({ pos,bossHeight,pos });
					m_shockWaves[13].first->SetPosition({ pos,bossHeight,-pos });
					m_shockWaves[14].first->SetPosition({ -pos,bossHeight,pos });
					m_shockWaves[15].first->SetPosition({ -pos,bossHeight,-pos });

					m_playShock = true;
					PlayEffect(m_shockWaves[8].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[9].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[10].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[11].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[12].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[13].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[14].first->GetWorldPosition());
					m_playShock = true;
					PlayEffect(m_shockWaves[15].first->GetWorldPosition());
				}

				m_shockCount++;
				m_shockWaveTime = 0.f;

				if (m_shockCount >= m_shockWavePos.size())
				{
					m_shockCount = 0;
					m_paternEnds = true;
					m_createComplete1 = false;
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
	if (!m_createComplete2)
	{
		for (int i = 0; i < 5; i++)
		{
			std::shared_ptr<Truth::Entity> flame = std::make_shared<Truth::Entity>(m_managers.lock());
			flame->Initialize();
			flame->m_layer = 1;
			flame->AddComponent<Truth::SphereCollider>();
			auto damage = flame->AddComponent<SimpleDamager>();
			damage->GetTypeInfo().GetProperty("damage")->Set(damage.get(), 3.f);

			flame->m_name = "Flame";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(flame);
			m_owner.lock()->AddChild(flame);

			flame->SetPosition({ 0.f,0.f,0.f });
			flame->SetScale({ 5.f,3.f,5.f });
			//flame->SetScale({ 300.f,30.f,300.f });

			flame->Start();

			m_fires.push_back(std::make_pair(flame, false));
		}
		m_createComplete2 = true;
	}
	else
	{
		if (m_readyToShoot)
		{
			float bossHeight = m_owner.lock()->m_transform->m_position.y;
			m_flameSwordTime += GetDeltaTime();
			if (m_flameSwordTime > 0.1f && m_flameCount < m_flamePos.size())
			{
				m_fires[m_flameCount].first->SetPosition({ 0.f,bossHeight,-m_flamePos[m_flameCount] });
				Vector3 worldPos = m_fires[m_flameCount].first->m_transform->m_worldPosition;
				m_owner.lock()->DeleteChild(m_fires[m_flameCount].first);
				m_owner.lock().reset();
				m_fires[m_flameCount].first->m_transform->m_position = worldPos;
				m_flameCount++;
				m_flameSwordTime = 0.f;

			}
			if (m_flameCount >= m_flamePos.size())
			{
				m_deleteFire = true;
				m_flameSword = false;
				m_flameSwordTime = 0.f;
				//m_bossAnimator->GetTypeInfo().GetProperty("isAttacking")->Set(m_bossAnimator.get(), false);
			}
		}
	}
}

void BossSkill::SwordShooting()
{
	if (!m_createComplete3)
	{
		m_swordShootTime += GetDeltaTime();
		if (m_swordShootTime > 0.1f && m_swordCount < m_swordPos.size())
		{
			std::shared_ptr<Truth::Entity> sword = std::make_shared<Truth::Entity>(m_managers.lock());
			sword->Initialize();
			sword->m_layer = 1;
			sword->AddComponent<Truth::BoxCollider>();
			auto damage = sword->AddComponent<SimpleDamager>();
			damage->GetTypeInfo().GetProperty("damage")->Set(damage.get(), 3.f);

			sword->m_name = "Sword";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(sword);
			m_owner.lock()->AddChild(sword);

			sword->SetPosition(m_swordPos[m_swordCount]);
			//sword->SetScale({ 30.f,30.f,300.f });
			sword->SetScale({ 1.5f,1.5f,3.f });


			sword->Start();

			m_swords.push_back(std::make_pair(sword, false));

			m_swordShootTime = 0.f;
			m_swordCount++;
		}
		if (m_swordCount >= m_swordPos.size())
		{
			m_createComplete3 = true;
			m_swordCount = 0;
			for (auto& e : m_swords)
			{
				Vector3 worldPos = e.first->m_transform->m_worldPosition;
				m_owner.lock()->DeleteChild(e.first);
				m_owner.lock().reset();
				e.first->m_transform->m_position = worldPos;
			}
		}
		m_spearImpactCount = 0;
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
			if (m_swords[i].second && m_spearImpactCount == i)
			{
				m_swords[i].first->m_transform->m_position += (m_shootingPos[i] - m_swords[i].first->m_transform->m_position) * GetDeltaTime() * 10.f;
				if (m_swords[i].first->m_transform->m_position.y <= m_owner.lock()->GetWorldPosition().y + 0.2f)
				{
					m_playSpear = true;
					m_spearImpactCount++;
					PlayEffect(m_swords[i].first->m_transform->m_position);
					m_swords[i].first->GetComponent<SimpleDamager>().lock()->GetTypeInfo().GetProperty("onlyHitOnce")->Set(m_swords[i].first->GetComponent<SimpleDamager>().lock().get(), true);
				}
			}
		}

		if (m_readyToShoot)
		{
			m_swordShootTime += GetDeltaTime();
			if (m_swordCount < m_swordPos.size() && m_swordShootTime > 1.f)
			{
				m_swords[m_swordCount].second = true;


				m_shootingPos.push_back(playerPos);
				m_swordCount++;

				m_swordShootTime = 0.f;
			}
		}

		if (m_swords[m_swords.size() - 1].first->m_transform->m_position.y <= m_owner.lock()->GetWorldPosition().y + 0.2f)
		{
			m_deleteSword = true;
			m_swordShooting = false;
			m_swordShootTime = 0.f;
			//m_bossAnimator->GetTypeInfo().GetProperty("isAttacking")->Set(m_bossAnimator.get(), false);
		}
	}

}

void BossSkill::LightSpeedDash(bool isSecondPhase)
{
	if (isSecondPhase)
	{
		if (!m_createComplete4)
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
				auto rigid = illusion->AddComponent<Truth::Controller>();
				auto damage = illusion->AddComponent<SimpleDamager>();
				damage->GetTypeInfo().GetProperty("damage")->Set(damage.get(), 3.f);
				skinnedMesh->SetSkinnedMesh(L"BossAnimations/Idle/Idle");
				skinnedMesh->AddAnimation("AttackLightSpeedReady", L"BossAnimations/Attacks/AttackLightSpeedReady");
				skinnedMesh->AddAnimation("AttackLightSpeedDash", L"BossAnimations/Attacks/AttackLightSpeedDash");
				illusion->m_name = "Illusion";
				m_managers.lock()->Scene()->m_currentScene->CreateEntity(illusion);
				m_owner.lock()->AddChild(illusion);

				rigid->GetTypeInfo().GetProperty("height")->Set(rigid.get(), 3.f);
				rigid->GetTypeInfo().GetProperty("radius")->Set(rigid.get(), 1.f);
				illusion->SetPosition(m_illusionPos[i]);
				illusion->SetScale({ 1.f,1.f,1.f });

				m_clones.push_back(std::make_pair(illusion, false));
				skinnedMesh->SetAnimation("AttackLightSpeedReady", false);
			}
			m_createComplete4 = true;

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

			if (m_cloneCount < m_clones.size())
			{
				if (m_readyToShoot)
				{
					m_lightSpeedDashTime += GetDeltaTime();

					if (m_lightSpeedDashTime > 0.8f)
					{
						auto skinnedMesh = m_clones[m_cloneCount].first->GetComponent<Truth::SkinnedMesh>().lock();

						m_clones[m_cloneCount].second = true;
						m_illusionPos[m_cloneCount] = playerPos;
						m_lightSpeedDashTime = 0.f;
						skinnedMesh->SetAnimation("AttackLightSpeedDash", false);
						skinnedMesh->SetAnimationSpeed(1.f);

						auto rigid = m_clones[m_cloneCount].first->GetComponent<Truth::Controller>().lock();

						Vector3 power(playerPos - m_clones[m_cloneCount].first->m_transform->m_position);
						power.y = 0.f;
						power.Normalize();
						power.y = -100.f;
						power *= GetDeltaTime();
						power *= 120.f;

						rigid->AddImpulse(power);

						m_cloneCount++;
					}
				}
			}

			for (int i = 0; i < m_clones.size(); i++)
			{
				auto skinnedMesh = m_clones[i].first->GetComponent<Truth::SkinnedMesh>().lock();

				if (!m_clones[i].second && skinnedMesh->GetTypeInfo().GetProperty("currentFrame")->Get<int>(skinnedMesh.get()).Get() > 142)
				{
					skinnedMesh->SetAnimationSpeed(0.f);

					/// 테스트
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
						skinnedMesh->SetAnimationSpeed(0.f);
						if (m_clones[1].second == false)
						{
							m_deleteClone = true;
						}
					}
				}
			}
		}
	}
}

void BossSkill::DistortedTimeSphere()
{
	if (!m_createComplete5)
	{
		for (int i = 0; i < 5; i++)
		{
			std::shared_ptr<Truth::Entity> timeSphere = std::make_shared<Truth::Entity>(m_managers.lock());
			timeSphere->Initialize();
			timeSphere->m_layer = 1;
			timeSphere->AddComponent<Truth::SphereCollider>();
			timeSphere->AddComponent<TimeDistortion>();
			timeSphere->m_name = "DistortedTimeSphere";
			m_managers.lock()->Scene()->m_currentScene->CreateEntity(timeSphere);
			m_owner.lock()->AddChild(timeSphere);

			timeSphere->SetPosition(m_spherePos[i]);
			timeSphere->SetScale({ 6.f,6.f,6.f });


			m_timeSpheres.push_back(std::make_pair(timeSphere, false));

		}

		m_createComplete5 = true;

		for (auto& e : m_timeSpheres)
		{
			Vector3 worldPos = e.first->m_transform->m_worldPosition;
			m_owner.lock()->DeleteChild(e.first);
			m_owner.lock().reset();
			e.first->m_transform->m_position = worldPos;
		}
	}
	else
	{
		if (m_bossAnimator->GetTypeInfo().GetProperty("timeDistortionCoolTime")->Get<bool>(m_bossAnimator.get()).Get())
		{
			for (auto& e : m_timeSpheres)
			{
				if (e.first->m_transform->m_position.y > m_owner.lock()->GetWorldPosition().y + 0.5f)
				{
					e.first->m_transform->m_position.y -= GetDeltaTime() * 2.f;
				}
				else
				{
					e.first->GetComponent<TimeDistortion>().lock()->GetTypeInfo().GetProperty("active")->Set(e.first->GetComponent<TimeDistortion>().lock().get(), true);
				}
			}
		}
	}
}

void BossSkill::CoolTimeCheck()
{
	// 칼쏘기
	if (m_bossAnimator->GetTypeInfo().GetProperty("swordShootCoolTime")->Get<bool>(m_bossAnimator.get()).Get())
	{
		m_swordShootCoolTime += GetDeltaTime();

		if (m_swordShootCoolTime > 10.f)
		{
			m_bossAnimator->GetTypeInfo().GetProperty("swordShootCoolTime")->Set(m_bossAnimator.get(), false);
			m_swordShootCoolTime = 0.f;
		}
	}

	// 충격파
	if (m_bossAnimator->GetTypeInfo().GetProperty("shockWaveCoolTime")->Get<bool>(m_bossAnimator.get()).Get())
	{
		m_shockWaveCoolTime += GetDeltaTime();

		if (m_shockWaveCoolTime > 10.f)
		{
			m_bossAnimator->GetTypeInfo().GetProperty("shockWaveCoolTime")->Set(m_bossAnimator.get(), false);
			m_shockWaveCoolTime = 0.f;
		}
	}

	// 불칼
	if (m_bossAnimator->GetTypeInfo().GetProperty("flameSwordCoolTime")->Get<bool>(m_bossAnimator.get()).Get())
	{
		m_flameSwordCoolTime += GetDeltaTime();

		if (m_flameSwordCoolTime > 10.f)
		{
			m_bossAnimator->GetTypeInfo().GetProperty("flameSwordCoolTime")->Set(m_bossAnimator.get(), false);
			m_flameSwordCoolTime = 0.f;
		}
	}

	// 분신돌진
	if (m_bossAnimator->GetTypeInfo().GetProperty("lightSpeedDashCoolTime")->Get<bool>(m_bossAnimator.get()).Get())
	{
		m_lightSpeedDashCoolTime += GetDeltaTime();

		if (m_lightSpeedDashCoolTime > 10.f)
		{
			m_bossAnimator->GetTypeInfo().GetProperty("lightSpeedDashCoolTime")->Set(m_bossAnimator.get(), false);
			m_lightSpeedDashCoolTime = 0.f;
		}
	}

	// 시왜구
	if (m_timeSpheres.empty())
	{
		m_createComplete5 = false;

		m_timeDistortionCoolTime += GetDeltaTime();

		if (m_timeDistortionCoolTime > 10.f)
		{
			m_bossAnimator->GetTypeInfo().GetProperty("timeDistortionCoolTime")->Set(m_bossAnimator.get(), false);
			m_timeDistortionCoolTime = 0.f;
		}
	}
}

void BossSkill::DeleteCheck()
{
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

		m_shockWaveTime = 0.f;
		m_flameSwordTime = 0.f;
		m_swordShootTime = 0.f;
		m_lightSpeedDashTime = 0.f;
		m_timeDistortionTime = 0.f;

		m_shockCount = 0;
		m_swordCount = 0;
		m_flameCount = 0;
		m_cloneCount = 0;

		m_createComplete1 = false;
		m_createComplete2 = false;
		m_createComplete3 = false;
		m_createComplete4 = false;
		m_createComplete5 = false;

		m_swordShooting = false;
		m_shockWave = false;
		m_flameSword = false;
		m_lightSpeedDash = false;
		m_timeDistortion = false;
	}

	if (m_deleteFire)
	{
		m_flameSword = false;
		m_flameSwordTime += GetDeltaTime();

		if (m_flameSwordTime > 5.f)
		{
			for (auto& e : m_fires)
			{
				m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
			}

			m_fires.clear();

			m_deleteFire = false;

			m_flameCount = 0;
			m_paternEnds = true;
			m_createComplete2 = false;
		}
	}

	if (m_deleteSword)
	{
		m_swordShooting = false;
		m_swordShootTime += GetDeltaTime();

		if (m_swordShootTime > 2.f)
		{
			for (auto& e : m_swords)
			{
				m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
			}

			m_swords.clear();
			m_shootingPos.clear();

			m_deleteSword = false;

			m_swordCount = 0;
			m_paternEnds = true;
			m_createComplete3 = false;
		}
	}

	if (m_deleteClone)
	{
		m_lightSpeedDashTime += GetDeltaTime();

		if (m_lightSpeedDashTime > 1.5f)
		{
			m_lightSpeedDash = false;

			for (auto& e : m_clones)
			{
				m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
			}

			m_clones.clear();
			m_illusionPos.clear();

			m_deleteClone = false;

			m_cloneCount = 0;
			m_paternEnds = true;
			m_createComplete4 = false;
			m_readyToShoot = false;
		}
	}

	if (!m_timeSpheres.empty())
	{
		int count = 0;
		for (auto& e : m_timeSpheres)
		{
			auto compo = e.first->GetComponent<TimeDistortion>().lock();
			if (compo->GetTypeInfo().GetProperty("delete")->Get<bool>(compo.get()).Get())
			{
				count++;
			}
		}

		if (count > 0)
		{
			for (auto& e : m_timeSpheres)
			{
				m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
			}
			m_timeSpheres.clear();
			m_deleteSphere = false;
			m_timeDistortion = false;
			m_timeDistortionCoolTime = 0.f;
		}
	}
}

void BossSkill::PlayEffect(Vector3 pos)
{
	if (m_playShock)
	{
		m_playShock = false;

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Beam.yaml");
			p->SetDeltaTime(GetDeltaTime());
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(0.4, 0.4, 1)) *
				Matrix::CreateRotationX(3.14f * 0.5f) *
				Matrix::CreateTranslation(pos)
			);

			p->SetActive(true);
			p->Play();
		}

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Beam1.yaml");
			p->SetDeltaTime(GetDeltaTime());
			p->SetTransformMatrix(
				Matrix::CreateScale(Vector3(0.4, 0.4, 1)) *
				Matrix::CreateRotationX(3.14f * 0.5f) *
				Matrix::CreateTranslation(pos)
			);

			p->SetActive(true);
			p->Play();
		}

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\Beam2.yaml");
			p->SetDeltaTime(GetDeltaTime());
			p->SetTransformMatrix(
				Matrix::CreateTranslation(pos)
			);

			p->SetActive(true);
			p->Play();
		}
	}

	if (m_playSpear)
	{
		m_playSpear = false;

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\SpearImpact.yaml");
			p->SetDeltaTime(GetDeltaTime());
			p->SetTransformMatrix(
				Matrix::CreateScale(35.f)
				* Matrix::CreateScale(Vector3(2.5, 2.5, 1) * 2.f)
				* Matrix::CreateRotationX(3.1415 * 0.5)
				* Matrix::CreateTranslation(pos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(3.f);

			p->Play();
		}

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\SpearImpact1.yaml");
			p->SetDeltaTime(GetDeltaTime());
			p->SetTransformMatrix(
				Matrix::CreateScale(35.f)
				* Matrix::CreateScale(Vector3(3, 3, 1) * 2.f)
				* Matrix::CreateRotationX(3.1415 * 0.5)
				* Matrix::CreateTranslation(pos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(3.f);

			p->Play();
		}

		{
			auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\SpearImpact2.yaml");
			p->SetDeltaTime(GetDeltaTime());
			p->SetTransformMatrix(
				Matrix::CreateScale(35.f)
				* Matrix::CreateScale(Vector3(1, 1, 2) * 2.f)
				* Matrix::CreateRotationX(3.1415 * 0.5)
				* Matrix::CreateTranslation(pos)
			);
			p->SetActive(true);
			p->SetSimulationSpeed(3.f);

			p->Play();
		}
	}

}

