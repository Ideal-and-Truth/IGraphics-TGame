#include "MenuBar.h"
#include "imgui.h"
#include "Scene.h"
#include "Managers.h"
#include "NewScenePopup.h"
#include "EditorUI.h"
#include "LoadMapDataPopup.h"

MenuBar::MenuBar(std::weak_ptr<Truth::Managers> _manager, HWND _hwnd, EditorUI* _editor)
	: EditorContext("Menu Bar", _manager, _hwnd, _editor)
{

}

MenuBar::~MenuBar()
{

}

void MenuBar::ShowContext(bool* p_open)
{
	// Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
	// Most functions would normally just assert/crash if the context is missing.
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

	// Verify ABI compatibility between caller code and compiled version of Dear ImGui. This helps detects some build issues.
	IMGUI_CHECKVERSION();

	std::shared_ptr<Truth::Scene> currentScene = m_manager.lock()->Scene()->m_currentScene;
	const auto& currentSceneName = currentScene->m_name;
	const auto& currentSceneEntities = currentScene->m_entities;

	ImGui::BeginMainMenuBar();
	ImGui::Text("\t\t\t\t\t\t\t");

	ImGui::Button("Menu");

	if (ImGui::BeginPopupContextItem("menu", 0))
	{
		if (ImGui::Selectable("New Scene"))
		{
			m_editor->AddContext(
				std::make_shared<NewScenePopup>(m_manager, m_hwnd, m_editor)
			);
		}
		if (ImGui::Selectable("Save Scene"))
		{
			m_manager.lock()->Scene()->SaveCurrentScene();

		}
		if (ImGui::Selectable("Save as Scene"))
		{
			m_openFileName.lpstrDefExt = L"scene";
			if (GetSaveFileName(&m_openFileName) != 0)
			{
				std::wstring filepath = m_openFileName.lpstrFile;
				std::vector<std::wstring> f = StringConverter::split(filepath, L'\\');

				f.back().pop_back();
				f.back().pop_back();
				f.back().pop_back();
				f.back().pop_back();
				f.back().pop_back();
				f.back().pop_back();

				m_manager.lock()->Scene()->SaveAsScene(filepath);
				USES_CONVERSION;
				m_manager.lock()->Scene()->m_currentScene->m_name = W2A(f.back().c_str());
			}
		}
		if (ImGui::Selectable("Load Scene"))
		{
			m_openFileName.lpstrFilter = m_sceneFileFilter;
			if (GetOpenFileName(&m_openFileName) != 0)
			{
				std::wstring filepath = m_openFileName.lpstrFile;
				m_manager.lock()->Scene()->LoadSceneData(filepath);
			}
		}
		if (ImGui::Selectable("Load Map Data"))
		{
			m_editor->AddContext(
				std::make_shared<LoadMapDataPopup>(m_manager, m_hwnd, m_editor)
			);
		}
		if (ImGui::Selectable("Create Empty"))
		{
			currentScene->AddEntity(std::make_shared<Truth::Entity>(m_manager.lock()));
		}
		if (ImGui::Selectable("Load Entity"))
		{
			m_openFileName.lpstrDefExt = L"entity";
			m_openFileName.lpstrFilter = m_entityFileFilter;
			if (GetOpenFileName(&m_openFileName) != 0)
			{
				std::wstring filepath = m_openFileName.lpstrFile;
				std::vector<std::wstring> f = StringConverter::split(filepath, L'\\');

				std::shared_ptr<Truth::Entity> e;

				std::ifstream inputstream(f.back());
				boost::archive::text_iarchive inputArchive(inputstream);
				inputArchive >> e;
				e->SetManager(m_manager);

				currentScene->AddEntity(e);
			}
		}
		ImGui::EndPopup();
	}

	if (ImGui::Button("Play"))
	{
		m_manager.lock()->EditToGame();
	}
	if (ImGui::Button("Stop"))
	{
		m_manager.lock()->GameToEdit();
	}

	static float step = 1.0f;
	static float dtdis = 1.0f;
	ImGui::Text("frame : %.2f\t", 1 / dtdis);
	if (step < 0.0f)
	{
		dtdis = m_manager.lock()->Time()->GetADT();
		step += 1.0f;
	}
	step -= dtdis;

	ImGui::EndMainMenuBar();
}
