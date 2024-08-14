#include "GraphicsEngine/Resource/UI/IdealText.h"
#include "GraphicsEngine/D2D/D2DTextManager.h"

Ideal::IdealText::IdealText()
{

}

Ideal::IdealText::~IdealText()
{

}

void Ideal::IdealText::SetText(std::wstring& Text)
{
	m_text = Text;
	m_isChanged = true;
}

