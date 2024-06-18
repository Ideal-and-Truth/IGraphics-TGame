#pragma once
#include <memory>
#include <vector>
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

public:
	EditorUI(std::shared_ptr<Truth::Managers> Manager);
	~EditorUI() {}

public:
	void RenderUI(bool* p_open);
	void ShowInspectorWindow(bool* p_open);
	void ShowHierarchyWindow(bool* p_open);
	void ShowMenuBar(bool* p_open);

	void TranslateComponent(std::shared_ptr<Truth::Component> EntityComponent);

	void AddComponentList(std::shared_ptr<Truth::Entity> SelectedEntity);

	void DisplayComponent(std::shared_ptr<Truth::Component> _component);
};


