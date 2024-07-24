#include "MeleeWeapon.h"
#include "BoxCollider.h"
#include "PlayerAnimator.h"
#include "Enemy.h"
#include "Player.h"

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
	m_playerAnimator = m_owner.lock()->m_parent.lock().get()->GetComponent<PlayerAnimator>().lock();
	m_player = m_owner.lock()->m_parent.lock().get()->GetComponent<Player>().lock();
}

void MeleeWeapon::Update()
{
	m_isAttacking = m_playerAnimator->GetTypeInfo().GetProperty("isAttacking")->Get<bool>(m_playerAnimator.get()).Get();

	if (!m_isAttacking)
	{
		for (auto& e : m_onHitEnemys)
		{
			auto enemy = e->GetComponent<Enemy>().lock().get();
			float playerDamage = m_player->GetTypeInfo().GetProperty("currentDamage")->Get<float>(m_player.get()).Get();
			float enemyHp = enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(enemy).Get();
			float hpLeft = enemyHp - playerDamage;
			enemy->GetTypeInfo().GetProperty("currentTP")->Set(enemy, hpLeft);
		}

		m_onHitEnemys.clear();
	}
}

void MeleeWeapon::OnTriggerEnter(Truth::Collider* _other)
{
	/// TODO : 이 구조는 온트리거스테이로 바꿔야함
	if (_other->GetOwner().lock()->GetComponent<Enemy>().lock())
	{
		if (m_isAttacking && _other->GetOwner().lock() != m_owner.lock()->m_parent.lock())
		{
			m_onHitEnemys.push_back(_other->GetOwner().lock());
		}
	}
}

void MeleeWeapon::OnTriggerExit(Truth::Collider* _other)
{

}
