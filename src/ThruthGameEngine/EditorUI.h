#pragma once
#include <memory>
#include <vector>
#include <string>
#include <windows.h>
#include "Types.h"
#include <queue>

#include "Animator.h"

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
	// static int32 m_selectedEntity;
	int m_notUsedID;

	std::vector<const char*>& m_componentList;

	char inputTextBuffer[256];

	OPENFILENAME m_openFileName;
	TCHAR m_filePathBuffer[256] = L"";
	TCHAR m_fileBuffer[256] = L"";
	const TCHAR* m_sceneFileFilter = L"Scene 파일\0*.scene";
	const TCHAR* m_entityFileFilter = L"Entity 파일\0*.entity";

	HWND m_hwnd;

	std::queue<std::pair<std::weak_ptr<Truth::Entity>, int>> m_deletedComponent;
	std::queue<std::weak_ptr<Truth::Entity>> m_createdEntity;

	std::weak_ptr<Truth::Entity> m_selectedEntity;
	std::weak_ptr<Truth::Animator> m_selectedAnimator;


	

public:
	EditorUI(std::shared_ptr<Truth::Managers> Manager, HWND _hwnd);
	~EditorUI() {}
	void RenderUI(bool* p_open);

private:
	void ShowInspectorWindow(bool* p_open);
	void ShowHierarchyWindow(bool* p_open);
	void ShowContentsDrawerWindow(bool* p_open);
	void ShowMenuBar(bool* p_open);
	void ShowAnimator(bool* p_open);

	void TranslateComponent(std::shared_ptr<Truth::Component> EntityComponent);

	void AddComponentList(std::shared_ptr<Truth::Entity> SelectedEntity);

	void DisplayComponent(std::shared_ptr<Truth::Component> _component);

	void DisplayEntity(std::weak_ptr<Truth::Entity> _entity);


};



//};
