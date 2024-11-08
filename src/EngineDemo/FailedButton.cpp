#include "FailedButton.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "Player.h"
#include "UI.h"

BOOST_CLASS_EXPORT_IMPLEMENT(FailedButton)

FailedButton::FailedButton()
	: m_speed(0.5f)
	, m_alpha(0.0f)
{
	m_name = "fail Button";
}

FailedButton::~FailedButton()
{

}

void FailedButton::Start()
{
	auto playerEntity = m_managers.lock()->Scene()->m_currentScene->FindEntity("Player");
	if (!playerEntity.expired())
		m_player = playerEntity.lock()->GetComponent<Player>();
	m_UI.lock()->SetAlpha(m_alpha);
}

void FailedButton::Update()
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

void FailedButton::OnMouseUp()
{
	if (!m_player.expired() && m_player.lock()->GetCurrentTP() <= 0.0f)
	{
		if (m_alpha < 0.8f)
		{
			return;
		}

		if (m_UI.lock()->GetName() == "OK")
		{
			m_managers.lock()->Scene()->ResetScene();
		}
		else if (m_UI.lock()->GetName() == "NO")
		{
			m_managers.lock()->Scene()->ChangeScene("TitleScene");
		}
	}
}
