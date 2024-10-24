#include "PlayerHPUI.h"
#include "Entity.h"
#include "Player.h"
#include "UI.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerHPUI)

PlayerHPUI::PlayerHPUI()
{
	m_name = "Player Cp Behavior";
}

void PlayerHPUI::Update()
{
	if (!m_player.expired())
	{
		float maxTP = m_player.lock()->GetMaxTP();
		float currentTP = m_player.lock()->GetCurrentTP();
		float rate;

		if (maxTP == 0.f)
			rate = 0.f;
		else
			rate = currentTP / maxTP;

		if (currentTP < 0)
			rate = 0.f;

		m_UI.lock()->SetScale({ rate, 1.0f });

		m_UI.lock()->SetSampling({ 0.f, 0.f }, { rate, 1.f });
	}
}

void PlayerHPUI::Start()
{
	m_player = m_owner.lock()->m_parent.lock()->GetComponent<Player>();
	m_UI.lock()->SetOnlyUI();
}
