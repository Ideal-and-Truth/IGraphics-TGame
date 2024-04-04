#include "Processor.h"
#include "TimeManager.h"


Processor::Processor()
	: m_hwnd(nullptr)
	, m_msg()
{
	m_timeManager = std::make_unique<TimeManager>();
	m_inputManager = std::make_unique<InputManager>();
}

Processor::~Processor()
{
}

void Processor::Initialize(HINSTANCE _hInstance)
{
	CreateMainWindow(_hInstance);
	InitializeManager();
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
			// ���� �۵��ϴ� �κ�
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
	this->m_timeManager->Update();
	this->m_inputManager->Update();
}

void Processor::LateUpdate()
{

}

void Processor::FixedUpdate()
{

}

void Processor::Render()
{

}

void Processor::CreateMainWindow(HINSTANCE _hInstance, int _width, int _height)
{
	wchar_t szAppName[] = L"Truth Engine";

	// â ����
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

	// Ŭ���̾�Ʈ �������� ���� ũ��� �� Ŀ�� �Ѵ�. (�ܰ���, Ÿ��Ʋ ��)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(m_hwnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
		_newWidth, _newHeight, SWP_SHOWWINDOW);
}

void Processor::InitializeManager()
{
	m_timeManager->Initalize();
	m_inputManager->Initalize(m_hwnd);
}
