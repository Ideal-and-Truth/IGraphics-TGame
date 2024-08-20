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
#include "Bullet.h"

BOOST_CLASS_EXPORT_IMPLEMENT(RangedWeapon)

RangedWeapon::RangedWeapon()
	: m_isAttacking(false)
	, m_isShot(false)
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


	if (m_isAttacking && !m_isShot)
	{
// 		std::shared_ptr<Truth::Entity> bullet = std::make_shared<Truth::Entity>(m_managers.lock());
// 		bullet->Initialize();
// 		bullet->m_layer = 1;
// 		bullet->AddComponent<Truth::RigidBody>();
// 		bullet->AddComponent<Truth::BoxCollider>(false);
// 		bullet->AddComponent<Truth::Mesh>(L"DebugObject/debugCube");
// 
// 		auto bulletComponent = bullet->AddComponent<Bullet>();
// 		float enemyDamage = m_enemy->GetTypeInfo().GetProperty("currentDamage")->Get<float>(m_enemy.get()).Get();
// 		bulletComponent->GetTypeInfo().GetProperty("bulletDamage")->Set(bulletComponent.get(), enemyDamage);
// 
// 		bullet->m_name = "bullet";
// 		m_managers.lock()->Scene()->m_currentScene->CreateEntity(bullet);
// 		bullet->SetPosition(m_owner.lock()->m_transform->m_worldPosition);
// 
// 		bullet->Awake();
// 		bullet->Start();
// 		auto r = bullet->GetComponent<Truth::RigidBody>().lock().get();
// 
// 		Vector3 targetPos = m_enemy->GetTypeInfo().GetProperty("target")->Get<std::weak_ptr<Truth::Entity>>(m_enemy.get()).Get().lock()->m_transform->m_worldPosition;
// 		Vector3 myPos = m_owner.lock()->m_parent.lock()->m_transform->m_worldPosition;
// 		Vector3 direction = targetPos - myPos;
// 		direction.y = 0;
// 		direction.Normalize();
// 
// 		Vector3 power(direction);
// 		power *= 100000.f;
// 		r->AddImpulse(power);
// 
// 		m_bullets.push_back(std::make_pair(bullet, 0.f));

		m_isShot = true;
	}

	if (!m_isAttacking && m_isShot)
	{
		m_isShot = false;
	}

	for (auto& e : m_bullets)
	{
		e.second += GetDeltaTime();

		if (e.first->GetComponent<Bullet>().lock()->GetTypeInfo().GetProperty("isHit")->Get<bool>(e.first->GetComponent<Bullet>().lock().get()).Get())
		{
			e.second += 3.f;
		}

		if (e.second > 2.f)
		{
			m_managers.lock()->Scene()->m_currentScene->DeleteEntity(e.first);
			// m_bullets.erase(m_bullets.begin());
			m_bullets.erase(remove(m_bullets.begin(), m_bullets.end(), e));
		}
	}
}

