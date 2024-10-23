#include "PlayerCPUI.h"
#include "Entity.h"
#include "Player.h"
#include "UI.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PlayerCPUI)

PlayerCPUI::PlayerCPUI()
{
	m_name = "Player Cp Behavior";
}

void PlayerCPUI::Update()
{
	if (!m_player.expired())
	{
		float maxCP = m_player.lock()->GetMaxCP();
		float currentCP = m_player.lock()->GetCurrentCP();
		float rate;

		if (maxCP == 0.f)
			rate = 0.f;
		else
			rate = currentCP / maxCP;

		if (currentCP < 0)
			rate = 0.f;

		m_UI.lock()->SetScale({ rate, 1.0f });

		m_UI.lock()->SetSampling({ 0.f, 0.f }, { rate, 1.f });
	}
}

void PlayerCPUI::Start()
{
	m_player = m_owner.lock()->m_parent.lock()->GetComponent<Player>();
	m_UI.lock()->SetOnlyUI();
}