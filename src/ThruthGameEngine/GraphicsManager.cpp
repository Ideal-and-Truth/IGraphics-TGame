#include "GraphicsManager.h"
#include "IRenderScene.h"
#include "ISkinnedMeshObject.h"
#include "imgui.h"

Truth::GraphicsManager::GraphicsManager()
	: m_renderer(nullptr)
	, m_renderScene(nullptr)
	, m_aspect(1.0f)
{

}

Truth::GraphicsManager::~GraphicsManager()
{

}

void Truth::GraphicsManager::Initalize(HWND _hwnd, uint32 _wight, uint32 _height)
{
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12,
		&_hwnd,
		_wight,
		_height,
		m_assetPath[0],
		m_assetPath[1],
		m_assetPath[2]
	);

	m_renderer->Init();
	m_renderScene = m_renderer->CreateRenderScene();
	m_renderer->SetRenderScene(m_renderScene);

	m_aspect = static_cast<float>(_wight) / static_cast<float>(_height);
	SetImguiStyle();
}

void Truth::GraphicsManager::Finalize()
{

}

void Truth::GraphicsManager::Tick()
{
	m_renderer->Tick();
}

void Truth::GraphicsManager::Render()
{
	m_renderer->Render();
}

void Truth::GraphicsManager::SetImguiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	auto& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF(("E:\\Projects\\ITEngine\\IGraphics-TGame\\src\\Ruda-Bold.ttf"), 12);
	io.Fonts->AddFontFromFileTTF(("E:\\Projects\\ITEngine\\IGraphics-TGame\\src\\Ruda-Bold.ttf"), 10);
	io.Fonts->AddFontFromFileTTF(("E:\\Projects\\ITEngine\\IGraphics-TGame\\src\\Ruda-Bold.ttf"), 14);
	io.Fonts->AddFontFromFileTTF(("E:\\Projects\\ITEngine\\IGraphics-TGame\\src\\Ruda-Bold.ttf"), 18);

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	// style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	// style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	// style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	// style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	// style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	// style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	// style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	// style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);



}

void Truth::GraphicsManager::AddObject(std::shared_ptr<Ideal::ISkinnedMeshObject> _mesh)
{
	m_renderScene->AddObject(_mesh);
}

std::shared_ptr<Ideal::ISkinnedMeshObject> Truth::GraphicsManager::CreateSkinnedMesh(std::wstring _path)
{
	return m_renderer->CreateSkinnedMeshObject(_path);
}

std::shared_ptr<Ideal::ICamera> Truth::GraphicsManager::CreateCamera()
{
	return m_renderer->CreateCamera();
}

void Truth::GraphicsManager::SetMainCamera(std::shared_ptr<Ideal::ICamera> _camera)
{
	m_renderer->SetMainCamera(_camera);
}

