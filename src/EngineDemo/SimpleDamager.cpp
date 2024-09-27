#include "SimpleDamager.h"
#include "Player.h"

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
}

void SimpleDamager::Update()
{
	if (m_isPlayerIn&& !m_onlyHitOnce)
	{
		float currentTP = m_player->GetTypeInfo().GetProperty("currentTP")->Get<float>(m_player.get()).Get();
		m_player->GetTypeInfo().GetProperty("currentTP")->Set(m_player.get(), currentTP - m_damage);
		m_onlyHitOnce = true;
	}
}

void SimpleDamager::OnTriggerEnter(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_isPlayerIn = true;
	}
}

void SimpleDamager::OnTriggerExit(Truth::Collider* _other)
{
	if (_other->GetOwner().lock()->m_name == "Player")
	{
		m_isPlayerIn = false;
	}
}
