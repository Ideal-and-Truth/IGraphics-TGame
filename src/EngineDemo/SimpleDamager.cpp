#include "SimpleDamager.h"
#include "Player.h"
#include "PlayerAnimator.h"
#include "Enemy.h"

BOOST_CLASS_EXPORT_IMPLEMENT(SimpleDamager)

SimpleDamager::SimpleDamager()
	: m_isPlayerIn(false)
	, m_onlyHitOnce(false)
	, m_damage(0.f)
{
	m_name = "SimpleDamager";
}

SimpleDamager::~SimpleDamager()
{

}

void SimpleDamager::Start()
{
	auto player = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
	m_player = player.lock()->GetComponent<Player>().lock();
	m_onlyHitOnce = false;
	if (!m_user)
	{
		m_user = m_owner.lock();
	}
}

void SimpleDamager::Update()
{
	if (m_isPlayerIn && !m_onlyHitOnce)
	{
		auto player = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
		auto playerAnimator = player.lock()->GetComponent<PlayerAnimator>().lock();

		if (!playerAnimator->GetTypeInfo().GetProperty("isDodge")->Get<bool>(playerAnimator.get()).Get())
		{
			float currentTP = m_player->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_player.get()).Get();
			float damage = m_damage;
			if (playerAnimator->GetTypeInfo().GetProperty("isGuard")->Get<bool>(playerAnimator.get()).Get())
			{
				damage *= 0.3f;
			}
			if (playerAnimator->GetTypeInfo().GetProperty("parry")->Get<bool>(playerAnimator.get()).Get())
			{
				damage = 0.f;
			}
			m_player->GetTypeInfo().GetProperty("currentTP")->Set(m_player.get(), currentTP - damage);
		}

		m_isPlayerIn = false;
		m_onlyHitOnce = true;
		m_damage = 0.f;
	}
}

void SimpleDamager::OnTriggerEnter(Truth::Collider* _other)
{
	if (m_user)
	{
		if (m_user->m_name == "Player")
		{
			if (_other->GetOwner().lock()->GetComponent<Enemy>().lock())
			{
				auto enemy = _other->GetOwner().lock()->GetComponent<Enemy>().lock();
				float currentTP = enemy->GetTypeInfo().GetProperty("currentTP")->Get<float>(enemy.get()).Get();
				enemy->GetTypeInfo().GetProperty("currentTP")->Set(enemy.get(), currentTP - m_damage);
			}
		}

		if (m_user->GetComponent<Enemy>().lock())
		{
			if (_other->GetOwner().lock()->m_name == "Player")
			{
				m_isPlayerIn = true;
			}
		}
	}
}

void SimpleDamager::OnTriggerExit(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_isPlayerIn = false;
	}
}

void SimpleDamager::OnCollisionEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_isPlayerIn = true;
	}
}

void SimpleDamager::OnCollisionExit(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_isPlayerIn = false;
	}
}
