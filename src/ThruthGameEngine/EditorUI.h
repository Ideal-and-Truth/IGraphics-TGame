#pragma once
#include "Managers.h"

class EditorUI
{
public:
	EditorUI(std::shared_ptr<Truth::Managers> Manager);
	~EditorUI() {}
public:
	void ShowInspectorWindow(bool* p_open);
	void ShowHierarchyWindow(bool* p_open);
	
private:
	std::shared_ptr<Truth::Managers> m_manager;
};

