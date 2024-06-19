#pragma once
#include <memory>
#include <vector>
#include <string>
#include <windows.h>
#include "Types.h"

namespace Truth
{
	class Entity;
	class Transform;
	class Managers;
	class Component;
}

class EditorUI
{
private:
	std::shared_ptr<Truth::Managers> m_manager;
	static int32 m_selectedEntity;
	int m_notUsedID;

	std::vector<const char*>& m_componentList;

	char inputTextBuffer[128];

	OPENFILENAME m_openFileName;
	TCHAR m_filePathBuffer[128] = L"";
	TCHAR m_fileBuffer[128] = L"";
	const TCHAR* m_fileFilter = L"Scene ÆÄÀÏ\0*.scene";

	HWND m_hwnd;

public:
	EditorUI(std::shared_ptr<Truth::Managers> Manager, HWND _hwnd);
	~EditorUI() {}
	void RenderUI(bool* p_open);

private:
	void ShowInspectorWindow(bool* p_open);
	void ShowHierarchyWindow(bool* p_open);
	void ShowMenuBar(bool* p_open);

	void TranslateComponent(std::shared_ptr<Truth::Component> EntityComponent);

	void AddComponentList(std::shared_ptr<Truth::Entity> SelectedEntity);

	void DisplayComponent(std::shared_ptr<Truth::Component> _component);
};


