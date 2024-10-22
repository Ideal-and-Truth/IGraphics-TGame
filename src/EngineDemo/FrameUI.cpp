#include "FrameUI.h"
#include "Managers.h"
#include "TimeManager.h"
#include "TextUI.h"

BOOST_CLASS_EXPORT_IMPLEMENT(FrameUI)

FrameUI::FrameUI()
	: m_interval(1.0f)
	, m_adt(0.0f)
	, m_frameCount(0)
{
	m_name = "FrameUI";
}

void FrameUI::Update()
{
	m_frameCount++;

	m_adt += m_managers.lock()->Time()->GetADT();
	if (m_adt >= m_interval)
	{
		m_TextUI.lock()->ChangeText(std::to_wstring(static_cast<uint32>(m_frameCount / m_interval)));
		m_frameCount = 0;
		m_adt -= m_interval;
	}

}

void FrameUI::Start()
{
}
