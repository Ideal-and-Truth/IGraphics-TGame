#include "ButtonTest.h"
#include "Logger.h"

BOOST_CLASS_EXPORT_IMPLEMENT(ButtonTest)

ButtonTest::ButtonTest()
{
	m_name = "testButton";
}

void ButtonTest::OnMouseClick()
{
	DEBUG_PRINT("CLICK\n");
}

void ButtonTest::OnMouseUp()
{
	DEBUG_PRINT("UP\n");
}

void ButtonTest::OnMouseOver()
{
	// DEBUG_PRINT("OVER");
}
