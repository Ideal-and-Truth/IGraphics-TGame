#include "EditorContext.h"

EditorContext::EditorContext(const std::string& _name, std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor)
	: m_name(_name)
	, m_manager(_manager)
	, m_hwnd(_hwnd)
	, m_editor(_editor)
{
}

EditorContext::~EditorContext()
{
}
