#include "Processor.h"
#include "Managers.h"
#include "SceneManager.h"
#include "TestScene.h"
#include "TestScene2.h"
#include "ISkinnedMeshObject.h"
#include "IRenderScene.h"
#include "IAnimation.h"
Processor::Processor()
	: m_hwnd(nullptr)
	, m_msg()
	, m_manager(nullptr)
	, m_wight(0)
	, m_height(0)
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
	CreateRender();
	//m_renderer->ConvertAssetToMyFormat(L"Tower/Tower.fbx");
	//mesh = m_renderer->CreateMeshObject(L"Tower/Tower");
	//m_renderer->ConvertAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx");
	//mesh = m_renderer->CreateMeshObject(L"CatwalkWalkForward3/CatwalkWalkForward3");

	m_manager->Scene()->AddScene<TestScene>("test", m_manager);
	m_manager->Scene()->AddScene<TestScene2>("test2", m_manager);

	m_manager->Scene()->SetCurrnetScene("test");

	m_manager->Scene()->StartGameScene();

	/// 그래픽 테스트
	m_renderScene = m_renderer->CreateRenderScene();
	m_renderer->SetRenderScene(m_renderScene);

	// m_renderer->ConvertAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx", true);
	// m_renderer->ConvertAssetToMyFormat(L"Kachujin/Mesh.fbx", true);

	m_cat = m_renderer->CreateSkinnedMeshObject(L"CatwalkWalkForward3/CatwalkWalkForward3");
	m_walkAnim = m_renderer->CreateAnimation(L"CatwalkWalkForward3/CatwalkWalkForward3");
	m_cat->AddAnimation("Walk", m_walkAnim);

	m_renderScene->AddObject(m_cat);

}

void Processor::Finalize()
{
	m_manager->Finalize();
}

void Processor::Process()
{
	Update();
	FixedUpdate();
	LateUpdate();
	Render();
}

void Processor::Loop()
{
	while (true)
	{
		if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_msg.message == WM_QUIT) break;

			DispatchMessage(&m_msg);
		}
		else
		{
			// 무언가 작동하는 부분
			Process();
		}
	}
}

LRESULT CALLBACK Processor::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Processor::Update()
{
	m_manager->Update();
	m_renderer->Tick();
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
	// m_manager->Render();
	m_renderer->Render();
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

	m_hwnd = CreateWindowW(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		100, 100, _width, _height, NULL, NULL, _hInstance, NULL);

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

	RECT newRect = {};
	newRect.left = 0;
	newRect.top = 0;
	newRect.right = _width;
	newRect.bottom = _height;

	//AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);
	//AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);

	// 클라이언트 영역보다 윈도 크기는 더 커야 한다. (외곽선, 타이틀 등)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(m_hwnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
		_newWidth, _newHeight, SWP_SHOWWINDOW);
}

void Processor::InitializeManager()
{
	m_manager = std::make_shared<Managers>();
	m_manager->Initialize(m_hwnd);
}

void Processor::CreateRender()
{
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12,
		&m_hwnd,
		m_wight,
		m_height,
		L"../Resources/Assets/",
		L"../Resources/Models/",
		L"../Resources/Textures/"
	);

	m_renderer->Init();
}
