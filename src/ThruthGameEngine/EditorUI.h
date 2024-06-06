#pragma once
#include "Managers.h"

class Entity;
class Transform;

class EditorUI
{
private:
	std::shared_ptr<Truth::Managers> m_manager;
	static int32 m_selectedEntity;
	int m_notUsedID;

public:
	EditorUI(std::shared_ptr<Truth::Managers> Manager);
	~EditorUI() {}

public:
	void RenderUI(bool* p_open);
	void ShowInspectorWindow(bool* p_open);
	void ShowHierarchyWindow(bool* p_open);
	

	//void TranslateProperty(const TypeInfo& value);
	void TranslateComponent(std::shared_ptr<Truth::Component> EntityComponent);
	void TransformUI(std::shared_ptr<Truth::Component> TransformComponent);
	void RigidbodyUI(std::shared_ptr<Truth::Component> RigidbodyComponent);
	void CameraUI(std::shared_ptr<Truth::Component> CameraComponent);
	void MeshFilterUI(std::shared_ptr<Truth::Component> MeshFilterComponent);
	void MeshRendererUI(std::shared_ptr<Truth::Component> MeshFilterComponent);
	void BoxColliderUI(std::shared_ptr<Truth::Component> ColliderComponent);
	void SphereColliderUI(std::shared_ptr<Truth::Component> ColliderComponent);
	void CapsuleColliderUI(std::shared_ptr<Truth::Component> ColliderComponent);
	void ScriptUI(std::shared_ptr<Truth::Component> UserMadeComponent);

};

