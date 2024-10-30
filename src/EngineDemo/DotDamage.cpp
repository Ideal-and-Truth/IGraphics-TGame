#include "DotDamage.h"
#include "Player.h"
#include "PlayerAnimator.h"
#include "Enemy.h"

BOOST_CLASS_EXPORT_IMPLEMENT(DotDamage)

DotDamage::DotDamage()
{
	m_name = "DotDamage";
}

DotDamage::~DotDamage()
{
	m_player->GetTypeInfo().GetProperty("onFire")->Set(m_player.get(), false);
}

void DotDamage::Start()
{
	auto player = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
	m_player = player.lock()->GetComponent<Player>().lock();
}

void DotDamage::Update()
{
	
}

void DotDamage::OnTriggerEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_player->GetTypeInfo().GetProperty("onFire")->Set(m_player.get(), true);
	}
}

void DotDamage::OnTriggerExit(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_player->GetTypeInfo().GetProperty("onFire")->Set(m_player.get(), false);
	}
}

