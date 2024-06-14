#include "EditorUI.h"
#include "GraphicsManager.h"
#include "imgui.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Entity.h"
// #include "EmptyEntity.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "Mesh.h"
#include "RigidBody.h"
#include "Camera.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "InputManager.h"

int32 EditorUI::m_selectedEntity = -1;

EditorUI::EditorUI(std::shared_ptr<Truth::Managers> Manager)
	: m_manager(Manager)
	, m_notUsedID(0)
{
	m_selectedEntity = -1;
}

void EditorUI::RenderUI(bool* p_open)
{
	ShowMenuBar(p_open);
	ShowInspectorWindow(p_open);
	ShowHierarchyWindow(p_open);
}

void EditorUI::ShowInspectorWindow(bool* p_open)
{
	// Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
	// Most functions would normally just assert/crash if the context is missing.
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

	// Verify ABI compatibility between caller code and compiled version of Dear ImGui. This helps detects some build issues.
	IMGUI_CHECKVERSION();

	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;
	static bool no_docking = false;
	static bool unsaved_document = false;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (no_menu)            window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
	if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
	if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

	// We specify a default position/size in case there's no data in the .ini file.
	// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Inspector", p_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	// Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.
	// e.g. Use 2/3 of the space for widgets and 1/3 for labels (right align)
	//ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
	// e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);


	/// 여기부터 UI 만들기

	auto currentSceneEntities = m_manager->Scene()->m_currentScene.lock()->GetTypeInfo().GetProperty("entities");
	auto getEntities = currentSceneEntities->Get<std::vector<std::shared_ptr<Truth::Entity>>>(m_manager->Scene()->m_currentScene.lock().get());
	const auto& entities = getEntities.Get();

	/// TEST : 비어있는 엔티티 씬에 추가 할 수 있는지 해봄
	/// 나중에 방법 알아내기
	if (m_manager->Input()->GetKeyState(KEY::ENTER) == KEY_STATE::DOWN)
	{
		//m_manager->Scene()->m_currentScene.lock()->CreateEntity(std::make_shared<Truth::Entity>());
	}

	if (m_selectedEntity >= 0)
	{
		// Set Entity Name
		{
			std::string sEntityName = entities[m_selectedEntity]->GetTypeInfo().GetProperty("name")->Get<std::string>(entities[m_selectedEntity].get());
			char* cEntityName = (char*)sEntityName.c_str();
			bool isShown = true;
			ImGui::Checkbox("##1", &isShown);
			ImGui::SameLine();
			ImGui::InputText("##2", cEntityName, 128);
			if (m_manager->Input()->GetKeyState(KEY::ENTER) == KEY_STATE::DOWN)
			{
				sEntityName = std::string(cEntityName, cEntityName + strlen(cEntityName));
				entities[m_selectedEntity]->GetTypeInfo().GetProperty("name")->Set(entities[m_selectedEntity].get(), sEntityName);
			}
		}

		// Show Components
		for (auto& e : entities[m_selectedEntity]->m_components)
		{
			// Checking Component
			TranslateComponent(e);
		}

		// Add Component
		{
			// Show Components List
			AddComponentList(entities[m_selectedEntity]);
		}
	}


	/// End of ShowDemoWindow()
	ImGui::PopItemWidth();
	ImGui::End();

}

void EditorUI::ShowHierarchyWindow(bool* p_open)
{
	// Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
	// Most functions would normally just assert/crash if the context is missing.
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

	// Verify ABI compatibility between caller code and compiled version of Dear ImGui. This helps detects some build issues.
	IMGUI_CHECKVERSION();

	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;
	static bool no_docking = false;
	static bool unsaved_document = false;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (no_menu)            window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
	if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
	if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

	// We specify a default position/size in case there's no data in the .ini file.
	// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Hierarchy", p_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.

		ImGui::End();
		return;
	}

	// Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.
	// e.g. Use 2/3 of the space for widgets and 1/3 for labels (right align)
	//ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
	// e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);



	/// 여기부터 UI 만들기

	// Hierarchy UI
	{
		auto currentScene = m_manager->Scene()->m_currentScene.lock();
		const auto& currentSceneName = currentScene->GetTypeInfo().GetProperty("name")->Get<std::string>(currentScene.get()).Get();
		const auto& currentSceneEntities = currentScene->GetTypeInfo().GetProperty("entities")->Get<std::vector<std::shared_ptr<Truth::Entity>>>(currentScene.get()).Get();

		uint32 selectCount = 0;

		// Current Scene Name
		if (ImGui::CollapsingHeader(currentSceneName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::Selectable("Create Empty"))
				{
					currentScene->AddEntity(std::make_shared<Truth::Entity>());
				}

				ImGui::EndPopup();
			}
			uint32 overlapedCount = 2;
			std::string entityName1;
			std::string entityName2;

			// 중복 엔티티 이름에 아이디 추가
			for (auto& e : currentSceneEntities)
			{
				for (auto& f : currentSceneEntities)
				{
					if (e != f)
					{
						entityName1 = e->GetTypeInfo().GetProperty("name")->Get<std::string>(e.get()).Get();
						entityName2 = f->GetTypeInfo().GetProperty("name")->Get<std::string>(f.get()).Get();
						if (entityName1 == entityName2)
						{
							entityName2.insert(entityName2.length(), "##");
							entityName2.insert(entityName2.length(), StringConverter::ToString(overlapedCount));
							overlapedCount++;

							f->GetTypeInfo().GetProperty("name")->Set(f.get(), entityName2);
						}
					}
				}
			}

			for (auto& e : currentSceneEntities)
			{
				const auto& entityName = e->GetTypeInfo().GetProperty("name")->Get<std::string>(e.get()).Get();

				// Select Entity
				if (entityName != "DefaultCamera" && ImGui::Selectable(entityName.c_str(), m_selectedEntity == selectCount))
				{

					m_selectedEntity = selectCount;
				}

				// Entity's Popup Menu
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::Selectable("Delete"))
					{
						/// TODO : 엔티티 삭제방법을 모르겠음
						m_manager->Scene()->m_currentScene.lock()->DeleteEntity(e);
					}

					ImGui::EndPopup();
				}
				selectCount++;
			}

		}
	}

	/// End of ShowDemoWindow()
	ImGui::PopItemWidth();
	ImGui::End();
}

void EditorUI::ShowMenuBar(bool* p_open)
{
	// Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
	// Most functions would normally just assert/crash if the context is missing.
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

	// Verify ABI compatibility between caller code and compiled version of Dear ImGui. This helps detects some build issues.
	IMGUI_CHECKVERSION();

	ImGui::BeginMainMenuBar();

	float4 dt = m_manager->Time()->GetADT();
	ImGui::Text("frame : %.2f\t", 1 / dt);

	if (ImGui::Button("Play"))
	{
		m_manager->EditToGame();
	}
	if (ImGui::Button("Stop"))
	{
		m_manager->GameToEdit();
	}

	ImGui::EndMainMenuBar();
}

void EditorUI::TranslateComponent(std::shared_ptr<Truth::Component> EntityComponent)
{
	const auto& componentName = EntityComponent->GetTypeInfo().GetProperty("name")->Get<std::string>(EntityComponent.get()).Get();


	if (componentName == "Transform")
	{
		TransformUI(EntityComponent);
	}
	else if (componentName == "Rigidbody")
	{
		RigidbodyUI(EntityComponent);
	}
	else if (componentName == "Camera")
	{
		CameraUI(EntityComponent);
	}
	else if (componentName == "Mesh Filter")
	{
		MeshFilterUI(EntityComponent);
		MeshRendererUI(EntityComponent);
	}
	else if (componentName == "Box Collider")
	{
		BoxColliderUI(EntityComponent);
	}
	else if (componentName == "Sphere Collider")
	{
		SphereColliderUI(EntityComponent);
	}
	else if (componentName == "Capsule Collider")
	{
		CapsuleColliderUI(EntityComponent);
	}
	else
	{
		ScriptUI(EntityComponent);
	}

}

void EditorUI::TransformUI(std::shared_ptr<Truth::Component> TransformComponent)
{

	// 컴포넌트 이름
	const auto& componentName = TransformComponent->GetTypeInfo().GetProperty("name")->Get<std::string>(TransformComponent.get()).Get();

	if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Position UI
		float position[3] = {};
		Vector3 posVec3 = TransformComponent->GetTypeInfo().GetProperty("position")->Get<Vector3>(TransformComponent.get()).Get();

		position[0] = posVec3.x;
		position[1] = posVec3.y;
		position[2] = posVec3.z;

		ImGui::DragFloat3("Position", position, 0.01f);

		posVec3.x = position[0];
		posVec3.y = position[1];
		posVec3.z = position[2];
		TransformComponent->GetTypeInfo().GetProperty("position")->Set(TransformComponent.get(), posVec3);


		// Scale UI
		float scale[3] = {};
		Vector3 scaleVec3 = TransformComponent->GetTypeInfo().GetProperty("scale")->Get<Vector3>(TransformComponent.get()).Get();

		scale[0] = scaleVec3.x;
		scale[1] = scaleVec3.y;
		scale[2] = scaleVec3.z;

		ImGui::DragFloat3("Scale", scale, 0.01f);

		scaleVec3.x = scale[0];
		scaleVec3.y = scale[1];
		scaleVec3.z = scale[2];
		TransformComponent->GetTypeInfo().GetProperty("scale")->Set(TransformComponent.get(), scaleVec3);
	}
}

void EditorUI::RigidbodyUI(std::shared_ptr<Truth::Component> RigidbodyComponent)
{
	// 컴포넌트 이름
	const auto& componentName = RigidbodyComponent->GetTypeInfo().GetProperty("name")->Get<std::string>(RigidbodyComponent.get()).Get();

	if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			// 팝업 메뉴에 들어갈 코드
			if (ImGui::Selectable("Remove Component"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();   // 이거까지 한세트
		}
		// Mass
		float mass = RigidbodyComponent->GetTypeInfo().GetProperty("mass")->Get<float>(RigidbodyComponent.get()).Get();
		ImGui::DragFloat("Mass", &mass, 0.01f, 1e-07f, 1e+09f);
		RigidbodyComponent->GetTypeInfo().GetProperty("mass")->Set(RigidbodyComponent.get(), mass);

		// Drag
		float drag = RigidbodyComponent->GetTypeInfo().GetProperty("drag")->Get<float>(RigidbodyComponent.get()).Get();
		ImGui::DragFloat("Drag", &drag, 0.01f, 0.f, (float)3.42e+38);
		RigidbodyComponent->GetTypeInfo().GetProperty("drag")->Set(RigidbodyComponent.get(), drag);

		// Angular Drag
		float angularDrag = RigidbodyComponent->GetTypeInfo().GetProperty("angularDrag")->Get<float>(RigidbodyComponent.get()).Get();
		ImGui::DragFloat("Angular Drag", &angularDrag, 0.01f, 0.f, (float)3.42e+38);
		RigidbodyComponent->GetTypeInfo().GetProperty("angularDrag")->Set(RigidbodyComponent.get(), angularDrag);

		// Use Gravity
		bool useGravity = RigidbodyComponent->GetTypeInfo().GetProperty("useGravity")->Get<bool>(RigidbodyComponent.get()).Get();
		ImGui::Checkbox("Use Gravity", &useGravity);
		RigidbodyComponent->GetTypeInfo().GetProperty("useGravity")->Set(RigidbodyComponent.get(), useGravity);

		// Is Kinematic
		bool isKinematic = RigidbodyComponent->GetTypeInfo().GetProperty("isKinematic")->Get<bool>(RigidbodyComponent.get()).Get();
		ImGui::Checkbox("Is Kinematic", &isKinematic);
		RigidbodyComponent->GetTypeInfo().GetProperty("isKinematic")->Set(RigidbodyComponent.get(), isKinematic);

		if (ImGui::TreeNode("Constraints"))
		{
			// Freeze Position
			{
				std::vector<bool> vFreezePosition = RigidbodyComponent->GetTypeInfo().GetProperty("freezePosition")->Get<std::vector<bool>>(RigidbodyComponent.get()).Get();
				ImGui::Text("Freeze Position");

				bool cFreezePosition[3] = { vFreezePosition[0],vFreezePosition[1],vFreezePosition[2] };

				ImGui::SameLine();
				ImGui::Checkbox("X", &cFreezePosition[0]);
				ImGui::SameLine();
				ImGui::Checkbox("Y", &cFreezePosition[1]);
				ImGui::SameLine();
				ImGui::Checkbox("Z", &cFreezePosition[2]);

				vFreezePosition[0] = cFreezePosition[0];
				vFreezePosition[1] = cFreezePosition[1];
				vFreezePosition[2] = cFreezePosition[2];

				RigidbodyComponent->GetTypeInfo().GetProperty("freezePosition")->Set(RigidbodyComponent.get(), vFreezePosition);
			}

			// Freeze Rotation
			{
				std::vector<bool> vFreezeRotation = RigidbodyComponent->GetTypeInfo().GetProperty("freezeRotation")->Get<std::vector<bool>>(RigidbodyComponent.get()).Get();
				ImGui::Text("Freeze Rotation");

				bool cFreezeRotation[3] = { vFreezeRotation[0],vFreezeRotation[1],vFreezeRotation[2] };

				ImGui::SameLine();
				ImGui::Checkbox("X##2", &cFreezeRotation[0]);
				ImGui::SameLine();
				ImGui::Checkbox("Y##2", &cFreezeRotation[1]);
				ImGui::SameLine();
				ImGui::Checkbox("Z##2", &cFreezeRotation[2]);

				vFreezeRotation[0] = cFreezeRotation[0];
				vFreezeRotation[1] = cFreezeRotation[1];
				vFreezeRotation[2] = cFreezeRotation[2];

				RigidbodyComponent->GetTypeInfo().GetProperty("freezeRotation")->Set(RigidbodyComponent.get(), vFreezeRotation);
			}

			ImGui::TreePop();
			ImGui::Spacing();
		}
	}
}

void EditorUI::CameraUI(std::shared_ptr<Truth::Component> CameraComponent)
{
	// 컴포넌트 이름
	const auto& componentName = CameraComponent->GetTypeInfo().GetProperty("name")->Get<std::string>(CameraComponent.get()).Get();

	if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		static float _fov = 0.25f * 3.141592f;
		float _aspect = m_manager->Graphics()->GetAspect();
		static float _near = 1.f;
		static float _far = 100000.f;

		// Field of View
		ImGui::SliderFloat("Field of View", &_fov, 1e-05f, 179);
		ImGui::DragFloat("Near", &_near, 0.01f, 0.01f, (float)3.42e+38);
		ImGui::DragFloat("Far", &_far, 0.01f, 0.01f, (float)3.42e+38);

		if (_far < _near)
		{
			_far = _near + 0.01f;
		}

		if (_fov > 0.f && _near > 0.f && _far > 0.f)
		{
			auto met = CameraComponent->GetTypeInfo().GetMethod("SetLens");
			if (met)
			{
				met->Invoke<void>(CameraComponent.get(), _fov, _aspect, _near, _far);
			}
		}
		else
		{
			_fov = 0.25f * 3.141592f;
			_near = 1.f;
			_far = 100000.f;
		}

	}
}

void EditorUI::MeshFilterUI(std::shared_ptr<Truth::Component> MeshFilterComponent)
{
	// 컴포넌트 이름
	const auto& componentName = MeshFilterComponent->GetTypeInfo().GetProperty("name")->Get<std::string>(MeshFilterComponent.get()).Get();

	if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::wstring wsMeshPath = MeshFilterComponent->GetTypeInfo().GetProperty("path")->Get<std::wstring>(MeshFilterComponent.get()).Get();
		std::string sMeshPath = std::string().assign(wsMeshPath.begin(), wsMeshPath.end());
		char* cMeshPath = (char*)sMeshPath.c_str();
		ImGui::InputText("Mesh", cMeshPath, 128);

		wsMeshPath = std::wstring(cMeshPath, cMeshPath + strlen(cMeshPath));

		MeshFilterComponent->GetTypeInfo().GetProperty("path")->Set(MeshFilterComponent.get(), wsMeshPath);

		if (m_manager->Input()->GetKeyState(KEY::ENTER) == KEY_STATE::DOWN)
		{
			auto met = MeshFilterComponent->GetTypeInfo().GetMethod("Awake");
			if (met)
			{
				met->Invoke<void>(MeshFilterComponent.get());
			}
		}
	}
}

void EditorUI::MeshRendererUI(std::shared_ptr<Truth::Component> MeshFilterComponent)
{
	bool isRendering = MeshFilterComponent->GetTypeInfo().GetProperty("isRendering")->Get<bool>(MeshFilterComponent.get()).Get();
	ImGui::Checkbox("##3", &isRendering);
	MeshFilterComponent->GetTypeInfo().GetProperty("isRendering")->Set(MeshFilterComponent.get(), isRendering);
	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}

void EditorUI::BoxColliderUI(std::shared_ptr<Truth::Component> ColliderComponent)
{
	// 컴포넌트 이름
	const auto& componentName = ColliderComponent->GetTypeInfo().GetProperty("name")->Get<std::string>(ColliderComponent.get()).Get();

	if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 vecPos = ColliderComponent->GetTypeInfo().GetProperty("center")->Get<Vector3>(ColliderComponent.get()).Get();
		Vector3 vecSize = ColliderComponent->GetTypeInfo().GetProperty("size")->Get<Vector3>(ColliderComponent.get()).Get();

		float pos[3] = {};
		pos[0] = vecPos.x;
		pos[1] = vecPos.y;
		pos[2] = vecPos.z;

		float size[3] = {};
		size[0] = vecSize.x;
		size[1] = vecSize.y;
		size[2] = vecSize.z;

		if (ImGui::DragFloat3("Center", pos, 0.01f))
		{
			vecPos.x = pos[0];
			vecPos.y = pos[1];
			vecPos.z = pos[2];

			auto met = ColliderComponent->GetTypeInfo().GetMethod("SetCenter");
			if (met)
			{
				met->Invoke<void>(ColliderComponent.get(), vecPos);
			}
		}

		if (ImGui::DragFloat3("Size", size, 0.01f))
		{
			vecSize.x = size[0];
			vecSize.y = size[1];
			vecSize.z = size[2];

			auto met = ColliderComponent->GetTypeInfo().GetMethod("SetSize");
			if (met)
			{
				met->Invoke<void>(ColliderComponent.get(), vecSize);
			}
		}
	}
}

void EditorUI::SphereColliderUI(std::shared_ptr<Truth::Component> ColliderComponent)
{
	// 컴포넌트 이름
	const auto& componentName = ColliderComponent->GetTypeInfo().GetProperty("name")->Get<std::string>(ColliderComponent.get()).Get();

	if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		float radius = ColliderComponent->GetTypeInfo().GetProperty("radius")->Get<float>(ColliderComponent.get()).Get();
		ImGui::DragFloat("Radius", &radius, 0.01f);

		auto met = ColliderComponent->GetTypeInfo().GetMethod("SetRadius");
		if (met)
		{
			met->Invoke<void>(ColliderComponent.get(), radius);
		}
	}
}

void EditorUI::CapsuleColliderUI(std::shared_ptr<Truth::Component> ColliderComponent)
{
	// 컴포넌트 이름
	const auto& componentName = ColliderComponent->GetTypeInfo().GetProperty("name")->Get<std::string>(ColliderComponent.get()).Get();

	if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		float radius = ColliderComponent->GetTypeInfo().GetProperty("radius")->Get<float>(ColliderComponent.get()).Get();
		ImGui::DragFloat("Radius##2", &radius, 0.01f);

		auto met = ColliderComponent->GetTypeInfo().GetMethod("SetRadius");
		if (met)
		{
			met->Invoke<void>(ColliderComponent.get(), radius);
		}

		float height = ColliderComponent->GetTypeInfo().GetProperty("height")->Get<float>(ColliderComponent.get()).Get();
		ImGui::DragFloat("Height", &height, 0.01f);

		auto met2 = ColliderComponent->GetTypeInfo().GetMethod("SetHeight");
		if (met2)
		{
			met2->Invoke<void>(ColliderComponent.get(), height);
		}
	}
}

void EditorUI::ScriptUI(std::shared_ptr<Truth::Component> UserMadeComponent)
{
	// 컴포넌트 이름
	const auto& componentName = UserMadeComponent->GetTypeInfo().GetProperty("name")->Get<std::string>(UserMadeComponent.get()).Get();
	std::string label(componentName + " (Script)");

	if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto& e : UserMadeComponent->GetTypeInfo().GetProperties())
		{
			auto valueName = e->GetTypeInfo().GetFullName();
			std::string propertyName = e->GetName();

			// 자료형마다 해줘야됨...
			if (valueName == "bool" && propertyName != "canMultiple")
			{
				auto propertyValue = e->Get<bool>(UserMadeComponent.get()).Get();
				ImGui::Checkbox(propertyName.c_str(), &propertyValue);
				e->Set(UserMadeComponent.get(), propertyValue);
			}
			else if (valueName == "int")
			{
				auto propertyValue = e->Get<int>(UserMadeComponent.get()).Get();
				ImGui::DragInt(propertyName.c_str(), &propertyValue);
				e->Set(UserMadeComponent.get(), propertyValue);
			}
			else if (valueName == "float")
			{
				auto propertyValue = e->Get<float>(UserMadeComponent.get()).Get();
				ImGui::DragFloat(propertyName.c_str(), &propertyValue);
				e->Set(UserMadeComponent.get(), propertyValue);
			}
		}
	}
}

void EditorUI::AddComponentList(std::shared_ptr<Truth::Entity> SelectedEntity)
{
	static bool isOpenComponentMenu = false;
	int item_current = -1;
	if (ImGui::Button("Add Component"))
	{
		isOpenComponentMenu = !isOpenComponentMenu;
	}
	if (isOpenComponentMenu)
	{
		/// TODO : 모든 컴포넌트의 이름을 받아올 수 있게 하고
		/// 같은게 있는 지 없는 지 확인하고 
		/// 있다면 중복 가능한지 확인해서
		/// 중복 가능하면 추가
		/// 불가능하면 안 추가
		/// 없다면 그냥 추가
		/// New Script 기능은 추가할 수 있는걸까
		const char* items[] = { "BoxCollider", "CapsuleCollider", "SphereCollider", "RigidBody", "Mesh", "Camera" };
		ImGui::ListBox("Component", &item_current, items, IM_ARRAYSIZE(items), 4);

		switch (item_current)
		{
			// Add BoxCollider
		case 0:
			if (SelectedEntity->GetComponent<Truth::BoxCollider>().lock().get())
			{
				auto entityPtr = SelectedEntity->GetComponent<Truth::BoxCollider>().lock().get();
				if (entityPtr->GetTypeInfo().GetProperty("canMultiple")->Get<bool>(entityPtr).Get())
				{
					SelectedEntity->AddComponent<Truth::BoxCollider>();

					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
				else
				{
					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
			}
			else
			{
				SelectedEntity->AddComponent<Truth::BoxCollider>();

				isOpenComponentMenu = !isOpenComponentMenu;
				break;
			}
			break;

			// Add CapsuleCollider
		case 1:
			if (SelectedEntity->GetComponent<Truth::CapsuleCollider>().lock().get())
			{
				auto entityPtr = SelectedEntity->GetComponent<Truth::CapsuleCollider>().lock().get();
				if (entityPtr->GetTypeInfo().GetProperty("canMultiple")->Get<bool>(entityPtr).Get())
				{
					SelectedEntity->AddComponent<Truth::CapsuleCollider>();

					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
				else
				{
					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
			}
			else
			{
				SelectedEntity->AddComponent<Truth::CapsuleCollider>();

				isOpenComponentMenu = !isOpenComponentMenu;
				break;
			}
			break;

			// Add SphereCollider
		case 2:
			if (SelectedEntity->GetComponent<Truth::SphereCollider>().lock().get())
			{
				auto entityPtr = SelectedEntity->GetComponent<Truth::SphereCollider>().lock().get();
				if (entityPtr->GetTypeInfo().GetProperty("canMultiple")->Get<bool>(entityPtr).Get())
				{
					SelectedEntity->AddComponent<Truth::SphereCollider>();

					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
				else
				{
					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
			}
			else
			{
				SelectedEntity->AddComponent<Truth::SphereCollider>();

				isOpenComponentMenu = !isOpenComponentMenu;
				break;
			}
			break;

			// Add RigidBody
		case 3:
			if (SelectedEntity->GetComponent<Truth::RigidBody>().lock().get())
			{
				auto entityPtr = SelectedEntity->GetComponent<Truth::RigidBody>().lock().get();
				if (entityPtr->GetTypeInfo().GetProperty("canMultiple")->Get<bool>(entityPtr).Get())
				{
					SelectedEntity->AddComponent<Truth::RigidBody>();

					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
				else
				{
					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
			}
			else
			{
				SelectedEntity->AddComponent<Truth::RigidBody>();
				isOpenComponentMenu = !isOpenComponentMenu;
				break;
			}
			break;

			// Add Mesh
		case 4:
			if (SelectedEntity->GetComponent<Truth::Mesh>().lock().get())
			{
				auto entityPtr = SelectedEntity->GetComponent<Truth::Mesh>().lock().get();
				if (entityPtr->GetTypeInfo().GetProperty("canMultiple")->Get<bool>(entityPtr).Get())
				{
					SelectedEntity->AddComponent<Truth::Mesh>(L"DebugObject/debugCube");

					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
				else
				{
					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
			}
			else
			{
				SelectedEntity->AddComponent<Truth::Mesh>(L"DebugObject/debugCube");

				isOpenComponentMenu = !isOpenComponentMenu;
				break;
			}
			break;

			// Add Camera
		case 5:
			if (SelectedEntity->GetComponent<Truth::Camera>().lock().get())
			{
				auto entityPtr = SelectedEntity->GetComponent<Truth::Camera>().lock().get();
				if (entityPtr->GetTypeInfo().GetProperty("canMultiple")->Get<bool>(entityPtr).Get())
				{
					SelectedEntity->AddComponent<Truth::Camera>();
					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
				else
				{
					isOpenComponentMenu = !isOpenComponentMenu;
					break;
				}
			}
			else
			{
				SelectedEntity->AddComponent<Truth::Camera>();
				isOpenComponentMenu = !isOpenComponentMenu;
				break;
			}
			break;

		}
	}
}

