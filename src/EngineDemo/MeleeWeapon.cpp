#include "MeleeWeapon.h"
#include "BoxCollider.h"
#include "PlayerAnimator.h"
#include "Player.h"
#include "EnemyAnimator.h"
#include "BossAnimator.h"
#include "Enemy.h"
#include "Transform.h"
#include "ParticleManager.h"
#include "IParticleSystem.h"

BOOST_CLASS_EXPORT_IMPLEMENT(MeleeWeapon)


MeleeWeapon::MeleeWeapon()
	: m_collider(nullptr)
	, m_isAttacking(false)
{
	m_name = "MeleeWeapon";
}

MeleeWeapon::~MeleeWeapon()
{

}

void MeleeWeapon::Awake()
{

}

void MeleeWeapon::Start()
{
	m_collider = m_owner.lock().get()->GetComponent<Truth::BoxCollider>().lock();
	if (m_owner.lock()->m_parent.lock()->m_name == "Player")
	{
		m_player = m_owner.lock()->m_parent.lock().get()->GetComponent<Player>().lock();
		m_playerAnimator = m_owner.lock()->m_parent.lock().get()->GetComponent<PlayerAnimator>().lock();
	}
	else
	{
		m_enemy = m_owner.lock()->m_parent.lock().get()->GetComponent<Enemy>().lock();
		m_enemyAnimator = m_owner.lock()->m_parent.lock().get()->GetComponent<EnemyAnimator>().lock();
		m_bossAnimator = m_owner.lock()->m_parent.lock().get()->GetComponent<BossAnimator>().lock();
		auto target = m_enemy->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.get()).Get().lock();
		m_playerAnimator = target->GetComponent<PlayerAnimator>().lock();
	}
}

void MeleeWeapon::Update()
{
	if (m_player)
	{
		m_isAttacking = m_playerAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_playerAnimator.get()).Get();
	}
	else if (m_enemy)
	{
		if (m_enemyAnimator)
		{
			m_isAttacking = m_enemyAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_enemyAnimator.get()).Get();
		}
		else
		{
			m_isAttacking = m_bossAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_bossAnimator.get()).Get();
		}
	}

	for (auto& e : m_onHitEnemys)
	{
		if (m_player && e)
		{
			auto enemy = e->GetComponent<Enemy>().lock().get();
			if (enemy)
			{
				if (!enemy->GetTypeInfo().GetProperty("hitOnce")->Get<bool>(enemy).Get())
				{
					float playerDamage = m_player->GetTypeInfo().GetProperty("currentDamage")->Get<float>(m_player.get()).Get();
					float enemyHp = enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(enemy).Get();
					float enemyStunGuage = enemy->GetTypeInfo().GetProperty("stunGuage")->Get<float>(enemy).Get();
					float hpLeft = enemyHp - playerDamage;

					enemy->GetTypeInfo().GetProperty("currentTP")->Set(enemy, hpLeft);
					enemy->GetTypeInfo().GetProperty("stunGuage")->Set(enemy, enemyStunGuage + 1.f);
					enemy->GetTypeInfo().GetProperty("hitOnce")->Set(enemy, true);

					Vector3 pos = e->GetWorldPosition();
					pos.y += 1.f;
					PlayEffect(pos);
				}
			}
		}
		else if (m_enemy)
		{
			auto player = e->GetComponent<Player>().lock().get();
			float enemyDamage = m_enemy->GetTypeInfo().GetProperty("currentDamage")->Get<float>(m_enemy.get()).Get();
			float playerHp = player->GetTypeInfo().GetProperty("currentTP")->Get<float>(player).Get();
			if (m_playerAnimator->GetTypeInfo().GetProperty("isGuard")->Get<bool>(m_playerAnimator.get()).Get())
			{
				enemyDamage *= 0.3f;
			}
			if (m_playerAnimator->GetTypeInfo().GetProperty("parry")->Get<bool>(m_playerAnimator.get()).Get())
			{
				enemyDamage = 0.f;
			}

			float hpLeft = playerHp - enemyDamage;
			player->GetTypeInfo().GetProperty("currentTP")->Set(player, hpLeft);

			/// 여기에서 가드 이펙트 사용하기


			m_onHitEnemys.clear();
		}


	}

	if (!m_isAttacking && m_player)
	{
		for (auto& e : m_onHitEnemys)
		{
			auto enemy = e->GetComponent<Enemy>().lock().get();
			if (enemy)
			{
				if (enemy->GetTypeInfo().GetProperty("hitOnce")->Get<bool>(enemy).Get())
				{
					if (!m_player->GetTypeInfo().GetProperty("slowTime")->Get<bool>(m_player.get()).Get())
					{
						auto damage = m_player->GetTypeInfo().GetProperty("currentDamage")->Get<float>(m_player.get()).Get();
						m_player->GetTypeInfo().GetProperty("currentCP")->Set(m_player.get(), m_player->GetTypeInfo().GetProperty("currentCP")->Get<float>(m_player.get()).Get() + damage);
						m_player->GetTypeInfo().GetProperty("currentTP")->Set(m_player.get(), m_player->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_player.get()).Get() + damage);
					}

					enemy->GetTypeInfo().GetProperty("hitOnce")->Set(enemy, false);
				}
			}

		}
		m_onHitEnemys.clear();
	}
}

void MeleeWeapon::OnTriggerEnter(Truth::Collider* _other)
{
	if (m_isAttacking)
	{
		if (m_player && _other->GetOwner().lock() != m_owner.lock()->m_parent.lock())
		{
			if (_other->GetOwner().lock()->GetComponent<Enemy>().lock())
			{
				if (_other->GetOwner().lock()->GetComponent<Enemy>().lock()->GetTypeInfo().GetProperty("currentTP")->Get<float>(_other->GetOwner().lock()->GetComponent<Enemy>().lock().get()).Get() > 0.f)
				{
					m_onHitEnemys.push_back(_other->GetOwner().lock());
					WaitForSecondsRealtime(m_playerAnimator->GetTypeInfo().GetProperty("hitStopTime")->Get<float>(m_playerAnimator.get()).Get());
				}
			}
		}
		else if (m_enemy && _other->GetOwner().lock() != m_owner.lock()->m_parent.lock())
		{
			if (_other->GetOwner().lock()->GetComponent<Player>().lock())
			{
				if (!_other->GetOwner().lock()->GetComponent<PlayerAnimator>().lock()->GetTypeInfo().GetProperty("isDodge")->Get<bool>(_other->GetOwner().lock()->GetComponent<PlayerAnimator>().lock().get()).Get())
				{
					if (_other->GetOwner().lock()->GetComponent<Player>().lock()->GetTypeInfo().GetProperty("currentTP")->Get<float>(_other->GetOwner().lock()->GetComponent<Player>().lock().get()).Get() > 0.f)
					{
						m_onHitEnemys.push_back(_other->GetOwner().lock());
						if (m_bossAnimator)
						{
							WaitForSecondsRealtime(m_bossAnimator->GetTypeInfo().GetProperty("hitStopTime")->Get<float>(m_bossAnimator.get()).Get());
						}
						else
						{
							WaitForSecondsRealtime(m_enemyAnimator->GetTypeInfo().GetProperty("hitStopTime")->Get<float>(m_enemyAnimator.get()).Get());
						}
					}
				}
			}
		}
	}
}

void MeleeWeapon::OnTriggerExit(Truth::Collider* _other)
{

}

void MeleeWeapon::PlayEffect(Vector3 pos)
{
	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage0.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(pos)
		);
		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage0.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f) * Matrix::CreateRotationY(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(pos)
		);
		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage1.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(pos)
		);
		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage1.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f) * Matrix::CreateRotationY(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(pos)
		);
		p->SetActive(true);
		p->Play();
	}

	{
		auto p = m_managers.lock()->Particle()->GetParticle("..\\Resources\\Particles\\norDamage2.yaml");
		p->SetTransformMatrix(
			Matrix::CreateRotationX(1.57f)
			* Matrix::CreateScale(2.f)
			* Matrix::CreateTranslation(pos)
		);
		p->SetActive(true);
		p->Play();
	}
}
