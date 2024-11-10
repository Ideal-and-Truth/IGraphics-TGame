#include "TitleUI.h"
#include "TextUI.h"

BOOST_CLASS_EXPORT_IMPLEMENT(TitleUI)

TitleUI::TitleUI()
	: m_alpha(1.0f)
	, m_speed(0.5f)
{
	m_name = "Title UI";
}

TitleUI::~TitleUI()
{

}

void TitleUI::Update()
{
	float speed = m_speed * m_managers.lock()->Time()->GetDT();
	m_alpha += m_alphaDirection * speed;

	if (m_alpha > 1.000f)
	{
		m_alpha = 1.0f;
		m_alphaDirection *= -1.0f;
	}
	else if (m_alpha < 0.000f)
	{
		m_alpha = 0.0f;
		m_alphaDirection *= -1.0f;
	}
	
	m_TextUI.lock()->SetAlpha(m_alpha);

	if (m_managers.lock()->Input()->GetKeyState(KEY::ENTER) == KEY_STATE::DOWN)
	{
		m_managers.lock()->Scene()->ChangeScene("Stage1");
	}
}

void TitleUI::Start()
{

}
