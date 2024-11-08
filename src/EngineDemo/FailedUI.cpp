#include "FailedUI.h"
#include "UI.h"
#include "Entity.h"
#include "Player.h"

BOOST_CLASS_EXPORT_IMPLEMENT(FailedUI)

FailedUI::FailedUI()
	: m_speed(0.5f)
	, m_alpha(0.0f)
{
	m_name = "Failed UI";
}

FailedUI::~FailedUI()
{

}

void FailedUI::Start()
{
	auto playerEntity = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
	if (!playerEntity.expired())
		m_player = playerEntity.lock()->GetComponent<Player>();
	m_UI.lock()->SetAlpha(m_alpha);
	m_UI.lock()->SetOnlyUI();
}

void FailedUI::Update()
{
	if (!m_player.expired() && m_player.lock()->GetCurrentTP() <= 0.0f)
	{
		float speed = m_speed * m_managers.lock()->Time()->GetDT();

		m_alpha += speed;

		if (m_alpha >= 1.0f)
		{
			m_alpha = 1.0f;
		}

		m_UI.lock()->SetAlpha(m_alpha);
	}
}
