#include "MeleeWeapon.h"
#include "BoxCollider.h"
#include "PlayerAnimator.h"
#include "Player.h"
#include "EnemyAnimator.h"
#include "Enemy.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(MeleeWeapon)

MeleeWeapon::MeleeWeapon()
	: m_collider(nullptr)
	, m_playerAnimator(nullptr)
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
	}
}

void MeleeWeapon::Update()
{
	/// 이거좀 치워라
	if (m_isAttacking)
	{
		Vector3 angle = m_owner.lock()->m_parent.lock().get()->m_transform->m_rotation.ToEuler();
		angle.y += 3.55f * GetDeltaTime();
		m_owner.lock()->m_parent.lock().get()->m_transform->m_rotation = Quaternion::CreateFromYawPitchRoll(angle);
	}

	if (m_player)
	{
		m_isAttacking = m_playerAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_playerAnimator.get()).Get();
	}
	else if (m_enemy)
	{
		m_isAttacking = m_enemyAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_enemyAnimator.get()).Get();
	}

}

void MeleeWeapon::OnTriggerEnter(Truth::Collider* _other)
{
	if (m_isAttacking)
	{
		if (m_player)
		{
			if (_other->GetOwner().lock()->GetComponent<Enemy>().lock())
			{
				if (m_isAttacking && _other->GetOwner().lock() != m_owner.lock()->m_parent.lock())
				{
					m_onHitEnemys.push_back(_other->GetOwner().lock());
				}
			}
		}
		else if (m_enemy)
		{
			if (_other->GetOwner().lock()->GetComponent<Player>().lock())
			{
				if (m_isAttacking && _other->GetOwner().lock() != m_owner.lock()->m_parent.lock())
				{
					m_onHitEnemys.push_back(_other->GetOwner().lock());
				}
			}
		}
	}
}

void MeleeWeapon::OnTriggerExit(Truth::Collider* _other)
{
	for (auto& e : m_onHitEnemys)
	{
		if (m_player)
		{
			auto enemy = e->GetComponent<Enemy>().lock().get();
			float playerDamage = m_player->GetTypeInfo().GetProperty("currentDamage")->Get<float>(m_player.get()).Get();
			float enemyHp = enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(enemy).Get();
			float hpLeft = enemyHp - playerDamage;
			enemy->GetTypeInfo().GetProperty("currentTP")->Set(enemy, hpLeft);
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

			float hpLeft = playerHp - enemyDamage;
			player->GetTypeInfo().GetProperty("currentTP")->Set(player, hpLeft);
		}
	}

	m_onHitEnemys.clear();
}
