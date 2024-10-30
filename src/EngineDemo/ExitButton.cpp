#include "ExitButton.h"
#include "UI.h"

BOOST_CLASS_EXPORT_IMPLEMENT(ExitButton)

ExitButton::ExitButton()
{
	m_name = "Exit Button";
}

void ExitButton::Start()
{
	m_UI.lock()->SetButton();
}

void ExitButton::OnMouseUp()
{
	m_managers.lock()->EndGame();
}
