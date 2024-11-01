#include "StartButton.h"
#include "UI.h"

BOOST_CLASS_EXPORT_IMPLEMENT(StartButton)

StartButton::StartButton()
{
	m_name = "Start Button";
}

void StartButton::Start()
{
	m_UI.lock()->SetButton();
	m_name = "Start Button";
}

void StartButton::OnMouseUp()
{
	m_managers.lock()->Scene()->ChangeScene("Stage1");
}
