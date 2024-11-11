#include "ClearUI.h"
#include "UI.h"
#include "Entity.h"
#include "Enemy.h"

BOOST_CLASS_EXPORT_IMPLEMENT(ClearUI)

ClearUI::ClearUI()
	: m_speed(0.5f)
	, m_alpha(0.0f)
	, m_isShown(true)
{
	m_name = "Failed UI";
}

ClearUI::~ClearUI()
{

}

void ClearUI::Start()
{
	auto bossEntity = m_managers.lock()->Scene()->m_currentScene->FindEntity("Boss");
	if (!bossEntity.expired())
		m_boss = bossEntity.lock()->GetComponent<Enemy>();
	m_UI.lock()->SetAlpha(m_alpha);

	// m_managers.lock()->Event()->Subscribe("Boss Destroy", MakeListenerInfo(&ClearUI::Trigger));
}

void ClearUI::Trigger([[maybe_unused]] const void* _)
{
	m_isShown = false;
}

void ClearUI::Update()
{
	float speed = m_speed * m_managers.lock()->Time()->GetDT();
	if (!m_boss.expired() && m_boss.lock()->GetCurrentTP() <= 0.0f && m_isShown)
	{
		m_alpha += speed;

		if (m_alpha >= 1.0f)
		{
			m_alpha = 1.0f;
		}

		m_UI.lock()->SetAlpha(m_alpha);
	}
	else
	{
		m_alpha -= speed;

		if (m_alpha <= 0.0f)
		{
			m_alpha = 0.0f;
		}

		m_UI.lock()->SetAlpha(m_alpha);
		if (m_managers.lock()->Input()->GetKeyState(KEY::ENTER) == KEY_STATE::DOWN)
		{
			m_managers.lock()->Scene()->ChangeScene("TitleScene");
		}
	}
}
