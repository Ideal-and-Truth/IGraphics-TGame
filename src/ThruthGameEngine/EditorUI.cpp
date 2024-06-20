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
#include "Managers.h"

int32 EditorUI::m_selectedEntity = -1;

EditorUI::EditorUI(std::shared_ptr<Truth::Managers> Manager)
	: m_manager(Manager)
	, m_notUsedID(0)
	, m_componentList(TypeInfo::g_factory->m_componentList)
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
	const auto& entities = m_manager->Scene()->m_currentScene.lock()->m_entities;

	if (m_selectedEntity >= 0)
	{
		// Set Entity Name
		{
			std::string sEntityName = entities[m_selectedEntity]->m_name;
			char* cEntityName = (char*)sEntityName.c_str();
			bool isShown = true;
			ImGui::Checkbox("##1", &isShown);
			ImGui::SameLine();
			ImGui::InputText("##2", cEntityName, 128);
			if (m_manager->Input()->GetKeyState(KEY::ENTER) == KEY_STATE::DOWN)
			{
				sEntityName = std::string(cEntityName, cEntityName + strlen(cEntityName));
				entities[m_selectedEntity]->m_name = sEntityName;
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

	/// Hierarchy UI
	{
		std::shared_ptr<Truth::Scene> currentScene = m_manager->Scene()->m_currentScene.lock();
		const auto& currentSceneName = currentScene->m_name;
		const auto& currentSceneEntities = currentScene->m_entities;

		uint32 selectCount = 0;

		// Current Scene Name
		if (ImGui::CollapsingHeader(currentSceneName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::Selectable("Create Empty"))
				{
					currentScene->AddEntity(std::make_shared<Truth::Entity>(m_manager));
				}

				ImGui::EndPopup();
			}

			for (auto& e : currentSceneEntities)
			{
				const std::string entityName = e->m_name + "##" + std::to_string(e->m_ID);

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
						m_manager->Scene()->m_currentScene.lock()->DeleteEntity(e);
						m_selectedEntity = -1;
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

	std::shared_ptr<Truth::Scene> currentScene = m_manager->Scene()->m_currentScene.lock();
	const auto& currentSceneName = currentScene->m_name;
	const auto& currentSceneEntities = currentScene->m_entities;

	ImGui::BeginMainMenuBar();
	ImGui::Button("Menu");

	static bool newScene = false, saveScene = false, loadScene = false;

	if (ImGui::BeginPopupContextItem("menu", 0))
	{
		if (ImGui::Selectable("New Scene"))
		{
			newScene = true;
		}
		if (ImGui::Selectable("Save Scene"))
		{
			saveScene = true;
		}
		if (ImGui::Selectable("Load Scene"))
		{
			loadScene = true;
		}
		if (ImGui::Selectable("Create Empty"))
		{
			currentScene->AddEntity(std::make_shared<Truth::Entity>(m_manager));
		}
		ImGui::EndPopup();
	}

	if (ImGui::Button("Play"))
	{
		m_manager->EditToGame();
	}
	if (ImGui::Button("Stop"))
	{
		m_manager->GameToEdit();
	}

	float4 dt = m_manager->Time()->GetADT();
	ImGui::Text("frame : %.2f\t", 1 / dt);

	if (newScene)
	{
		ImGui::OpenPopup("Input String");
		if (ImGui::BeginPopupModal("Input String", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Enter your text below:");
			ImGui::InputText("##input", inputTextBuffer, IM_ARRAYSIZE(inputTextBuffer));

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				std::shared_ptr<Truth::Scene> ns = std::make_shared<Truth::Scene>(m_manager);
				ns->m_name = inputTextBuffer;
				m_manager->Scene()->SaveScene(ns);
				std::string path = "../Scene/" + ns->m_name + ".scene";
				m_manager->Scene()->LoadSceneData(path);
				m_manager->Scene()->SetCurrnetScene(ns->m_name);
				newScene = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				newScene = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	else if (saveScene)
	{
	}
	else if (loadScene)
	{
	}

	ImGui::EndMainMenuBar();
}

void EditorUI::TranslateComponent(std::shared_ptr<Truth::Component> EntityComponent)
{
	DisplayComponent(EntityComponent);
}

void EditorUI::AddComponentList(std::shared_ptr<Truth::Entity> SelectedEntity)
{

	if (ImGui::CollapsingHeader("Add Component"))
	{
		int selectedItem = -1;
		if (ImGui::ListBox("Component", &selectedItem, m_componentList.data(), static_cast<int32>(m_componentList.size()), 4))
		{
			SelectedEntity->AddComponent(TypeInfo::g_factory->Create(m_componentList[selectedItem]));
		}
	}
}

void EditorUI::DisplayComponent(std::shared_ptr<Truth::Component> _component)
{
	const TypeInfo& typeinfo = _component->GetTypeInfo();

	// 컴포넌트 이름
	const char* componentName = typeinfo.GetName();

	auto& properties = typeinfo.GetProperties();
	bool isSelect = false;

	const auto& entities = m_manager->Scene()->m_currentScene.lock()->m_entities;

	if (ImGui::CollapsingHeader(componentName, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::Selectable("Remove Component"))
			{
				entities[m_selectedEntity]->DeleteComponent(_component->m_index);
			}

			ImGui::EndPopup();
		}
		for (auto* p : properties)
		{
			isSelect |= p->DisplayUI(_component.get());
		}
	}
#ifdef _DEBUG
	if (isSelect)
	{
		_component->EditorSetValue();
	}
#endif // _DEBUG
}

