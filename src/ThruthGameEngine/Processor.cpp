 #include "Processor.h"
#include "Managers.h"
#include "SceneManager.h"
// #include "TestScene.h"
// #include "TestScene2.h"
#include "ISkinnedMeshObject.h"
#include "IRenderScene.h"
#include "IAnimation.h"
#include "GraphicsManager.h"
#include "imgui.h"
#include "InputManager.h"
//#include "../IdealGraphics/Misc/Utils/FileUtils.h"
#include "EditorUI.h"

#include "UnityParser.h"

#include <xxhash.h>

Ideal::IdealRenderer* Processor::g_Renderer = nullptr;
Truth::InputManager* Processor::g_inputmanager = nullptr;

Processor::Processor()
	: m_hwnd(nullptr)
	, m_msg()
	, m_manager(nullptr)
	, m_wight(0)
	, m_height(0)
	, m_editor(nullptr)
{
	DEBUG_PRINT("start process\n");
}

Processor::~Processor()
{
	Finalize();
	DEBUG_PRINT("end process\n");
}

/// <summary>
/// 프로세서 초기화
/// </summary>
/// <param name="_hInstance"></param>
void Processor::Initialize(HINSTANCE _hInstance)
{
	CreateMainWindow(_hInstance);
	InitializeManager();
	g_inputmanager = m_manager->Input().get();

	// 	g_Renderer->ConvertAssetToMyFormat(L"Kachujin/Mesh.fbx", true);
	// 	g_Renderer->ConvertAssetToMyFormat(L"debugObject/debugCube.fbx", false, true);
	// 	g_Renderer->ConvertAssetToMyFormat(L"debugObject/debugSphere.fbx", false, true);
	// 	g_Renderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Run.fbx");
	// 	g_Renderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Idle.fbx");
	// 	g_Renderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Slash.fbx");
	// g_Renderer->ConvertAnimationAssetToMyFormat(L"test2/testWalk2.fbx");// -> Assimp Converter에서 FLAG 해제
	// g_Renderer->ConvertAssetToMyFormat(L"TestMap/Map2.fbx");

	Truth::UnityParser up(m_manager->Graphics().get());

	// 	std::string buffer = "Type:Mesh->Sprite__3_0";
	// 	XXH64_hash_t hash = XXH64(buffer.c_str(), buffer.length(), 0);
	// 	int64 temp = static_cast<int64>(hash);
	// up.SetRootDir("E:\\Projects\\SampleUnity\\parsingTest");
	// up.ParseSceneFile("E:\\Projects\\SampleUnity\\parsingTest\\Assets\\Scenes\\SampleScene.unity");

	m_editor = std::make_unique<EditorUI>(m_manager, m_hwnd);
}

void Processor::Finalize()
{
	m_manager->Finalize();
}

void Processor::Process()
{
	Update();
	LateUpdate();
	Render();
}

void Processor::Loop()
{
	m_manager->Scene()->StartGameScene();
	while (true)
	{
		if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_msg.message == WM_QUIT) break;

			DispatchMessage(&m_msg);
			TranslateMessage(&m_msg);
		}
		else
		{
			// 무언가 작동하는 부분
			Process();
		}
	}
}

void Processor::LoadScene(std::wstring _path)
{
	m_manager->Scene()->LoadSceneData(_path);
}

LRESULT CALLBACK Processor::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;
	if (g_Renderer)
	{
		g_Renderer->SetImGuiWin32WndProcHandler(hWnd, message, wParam, lParam);
	}
	if (g_inputmanager)
	{
		g_inputmanager->ResetMouseMovement();
	}
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		if (g_Renderer)
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			//AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
			DWORD width = rect.right - rect.left;
			DWORD height = rect.bottom - rect.top;
			g_Renderer->Resize(width, height);
		}
		break;
	}

	case WM_MOUSEWHEEL:
	{
		if (g_inputmanager)
		{
			g_inputmanager->m_deltaWheel = GET_WHEEL_DELTA_WPARAM(wParam);
		}
		break;
	}

	default:
		if (g_inputmanager)
		{
			g_inputmanager->m_deltaWheel = 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Processor::SaveCurrentScene()
{
	m_manager->Scene()->SaveCurrentScene();
}

void Processor::SaveScene(std::shared_ptr<Truth::Scene> _scene)
{
	m_manager->Scene()->SaveScene(_scene);
}

void Processor::Update()
{
	m_manager->Update();
}

void Processor::LateUpdate()
{
	m_manager->LateUpdate();
}

void Processor::FixedUpdate()
{
	m_manager->FixedUpdate();
}

void Processor::Render()
{
#ifdef EDITOR_MODE
	if (g_Renderer)
	{
		g_Renderer->ClearImGui();
		//ImGui::ShowDemoWindow(&show_demo_window);
		m_editor->RenderUI(&show_demo_window);
	}
#endif // EDITOR_MODE

	m_manager->Render();
}

void Processor::CreateMainWindow(HINSTANCE _hInstance, uint32 _width, uint32 _height, const wchar_t szAppName[])
{
	m_wight = _width;
	m_height = _height;

	// 창 지정
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Processor::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(_hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szAppName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	RegisterClassExW(&wcex);

	RECT windowRect = { 0,0, 1920, 1080 };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	m_hwnd = CreateWindowW(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		100, 100, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, _hInstance, NULL);

	if (!m_hwnd)
	{
		return;
	}

	ShowWindow(m_hwnd, SW_SHOWNORMAL);
	UpdateWindow(m_hwnd);

	RECT nowRect;
	DWORD _style = (DWORD)GetWindowLong(m_hwnd, GWL_STYLE);
	DWORD _exstyle = (DWORD)GetWindowLong(m_hwnd, GWL_EXSTYLE);

	GetWindowRect(m_hwnd, &nowRect);

	// 	RECT newRect = {};
	// 	newRect.left = 0;
	// 	newRect.top = 0;
	// 	newRect.right = _width;
	// 	newRect.bottom = _height;
	// 
	// 	//AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);
	// 	//AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);
	// 
	// 	// 클라이언트 영역보다 윈도 크기는 더 커야 한다. (외곽선, 타이틀 등)
	// 	int _newWidth = (newRect.right - newRect.left);
	// 	int _newHeight = (newRect.bottom - newRect.top);
	// 
	// 	SetWindowPos(m_hwnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
	// 		_newWidth, _newHeight, SWP_SHOWWINDOW);
}

void Processor::InitializeManager()
{
	m_manager = std::make_shared<Truth::Managers>();
	m_manager->Initialize(m_hwnd, m_wight, m_height);
	g_Renderer = m_manager->Graphics()->GetRenderer().get();
	// g_Renderer->ConvertAssetToMyFormat(L"debugCube/debugCube.fbx");
}

