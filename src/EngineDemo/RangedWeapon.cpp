#include "RangedWeapon.h"
#include "Transform.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "Mesh.h"
#include "PlayerController.h"
#include "Player.h"
#include "PlayerAnimator.h"
#include "Enemy.h"
#include "EnemyAnimator.h"

BOOST_CLASS_EXPORT_IMPLEMENT(RangedWeapon)

RangedWeapon::RangedWeapon()
	: m_isAttacking(false)
{
	m_name = "RangedWeapon";
}

RangedWeapon::~RangedWeapon()
{

}

void RangedWeapon::Awake()
{

}

void RangedWeapon::Start()
{
	if (m_owner.lock()->m_parent.lock()->m_name == "Player")
	{
		m_player = m_owner.lock()->m_parent.lock().get()->GetComponent<Player>().lock();
		m_playerAnimator = m_owner.lock()->m_parent.lock().get()->GetComponent<PlayerAnimator>().lock();
	}
	else
	{
		m_enemy = m_owner.lock()->m_parent.lock().get()->GetComponent<Enemy>().lock();
		m_enemyAnimator = m_owner.lock()->m_parent.lock().get()->GetComponent<EnemyAnimator>().lock();
		auto target = m_enemy->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.get()).Get().lock();
		m_playerAnimator = target->GetComponent<PlayerAnimator>().lock();
	}
}

void RangedWeapon::Update()
{
	if (m_player)
	{
		m_isAttacking = m_playerAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_playerAnimator.get()).Get();
	}
	else if (m_enemy)
	{
		m_isAttacking = m_enemyAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_enemyAnimator.get()).Get();
	}


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

	/// TODO : 여기부터
	if (GetKeyDown(KEY::SPACE))
	{
		std::shared_ptr<Truth::Entity> bullet = std::make_shared<Truth::Entity>(m_managers.lock());
		bullet->m_layer = 1;
		bullet->AddComponent<Truth::RigidBody>();
		bullet->AddComponent<Truth::BoxCollider>(false);
		bullet->AddComponent<Truth::Mesh>(L"DebugObject/debugSphere");
		bullet->m_name = "bullet";
		m_managers.lock()->Scene()->m_currentScene->CreateEntity(bullet);
		bullet->Initailize();
		bullet->SetPosition(m_owner.lock()->m_transform->m_worldPosition);

		bullet->Awake();
		bullet->Start();
		auto r = bullet->GetComponent<Truth::RigidBody>().lock().get();
		Vector3 power(m_owner.lock()->m_parent.lock()->GetComponent<PlayerController>().lock()->GetTypeInfo().GetProperty("playerDirection")
			->Get<Vector3>(m_owner.lock()->m_parent.lock()->GetComponent<PlayerController>().lock().get()).Get());
		power *= 100000.f;
		r->AddImpulse(power);

		m_bullets.push_back(std::make_pair(bullet, 0.f));
	}


	for (auto& e : m_bullets)
	{
		e.second += GetDeltaTime();

		if (e.second > 2.f)
		{
			/// TODO : 삭제 고치면 삭제하기
			e.first->SetPosition(m_owner.lock()->m_transform->m_worldPosition);
			//m_managers.lock()->Scene()->m_currentScene->DeleteEntity(m_owner.lock());
			m_bullets.erase(m_bullets.begin());
		}
	}
}

void RangedWeapon::OnTriggerEnter(Truth::Collider* _other)
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

void RangedWeapon::OnTriggerExit(Truth::Collider* _other)
{

}
