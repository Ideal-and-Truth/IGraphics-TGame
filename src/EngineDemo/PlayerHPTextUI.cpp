#include "PlayerHPTextUI.h"
#include "Entity.h"
#include "Player.h"
#include "TextUI.h"
BOOST_CLASS_EXPORT_IMPLEMENT(PlayerHPTextUI)

PlayerHPTextUI::PlayerHPTextUI()
{
	m_name = "PlayerHPTextUI";
}

void PlayerHPTextUI::Update()
{
	if (m_player.expired())
		return;

	m_TextUI->ChangeText(std::to_wstring(m_player.lock()->GetCurrentTP()));
}

void PlayerHPTextUI::Start()
{
	m_player = m_owner->GetComponent<Player>();
}
