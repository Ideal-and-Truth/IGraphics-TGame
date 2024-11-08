#include "FailedUI.h"
#include "UI.h"
#include "Entity.h"
#include "Player.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::ButtonBehavior)

FailedUI::FailedUI()
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
	m_UI.lock()->SetAlpha(0.0f);
}

void FailedUI::Update()
{
	if (m_player.expired() && m_player.lock()->GetCurrentTP() <= 0.0f)
	{

	}
}
