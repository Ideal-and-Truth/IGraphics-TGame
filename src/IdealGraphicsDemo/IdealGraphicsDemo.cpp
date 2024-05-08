// IdealGraphicsDemo.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "IdealGraphicsDemo.h"

#ifdef _DEBUG
#pragma comment(lib, "DebugLib/GraphicsEngine/IdealGraphics.lib")
#else
#pragma comment(lib, "ReleaseLib/GraphicsEngine/IdealGraphics.lib")
#endif

#include "GraphicsEngine/public/IdealRendererFactory.h"
#include "GraphicsEngine/public/IdealRenderer.h"
#include "GraphicsEngine/public/IMeshObject.h"
#include "GraphicsEngine/public/ISkinnedMeshObject.h"
#include "GraphicsEngine/public/IAnimation.h"
#include "GraphicsEngine/public/IRenderScene.h"

//#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
//#include "GraphicsEngine/public/ICamera.h"
#include "../Utils/SimpleMath.h"
using namespace DirectX::SimpleMath;

#define MAX_LOADSTRING 100

#define WIDTH 1280
#define HEIGHT 960
HWND g_hWnd;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_IDEALGRAPHICSDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IDEALGRAPHICSDEMO));

	MSG msg = {};
	WCHAR programpath[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, programpath);
	{
		std::shared_ptr<Ideal::IdealRenderer> Renderer = CreateRenderer(
			EGraphicsInterfaceType::D3D12,
			&g_hWnd,
			WIDTH,
			HEIGHT,
			L"../Resources/Assets/",
			L"../Resources/Models/",
			L"../Resources/Textures/"
		);

		Renderer->Init();
		
		//-------------------Create Scene-------------------//
		std::shared_ptr<Ideal::IRenderScene> renderScene = Renderer->CreateRenderScene();
		Renderer->SetRenderScene(renderScene);

		//-------------------Convert FBX(Model, Animation)-------------------//
		//Renderer->ConvertAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx", true);
		//Renderer->ConvertAssetToMyFormat(L"Kachujin/Mesh.fbx", true);
		//Renderer->ConvertAnimationAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx");
		//Renderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Run.fbx");
		//Renderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Idle.fbx");
		//Renderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Slash.fbx");

		//-------------------Create Mesh Object-------------------//
		std::shared_ptr<Ideal::ISkinnedMeshObject> cat = Renderer->CreateSkinnedMeshObject(L"CatwalkWalkForward3/CatwalkWalkForward3");
		std::shared_ptr<Ideal::IAnimation> walkAnim = Renderer->CreateAnimation(L"CatwalkWalkForward3/CatwalkWalkForward3");
		std::shared_ptr<Ideal::ISkinnedMeshObject> ka = Renderer->CreateSkinnedMeshObject(L"Kachujin/Mesh");
		std::shared_ptr<Ideal::IAnimation> runAnim = Renderer->CreateAnimation(L"Kachujin/Run");
		std::shared_ptr<Ideal::IAnimation> idleAnim = Renderer->CreateAnimation(L"Kachujin/Idle");
		std::shared_ptr<Ideal::IAnimation> slashAnim = Renderer->CreateAnimation(L"Kachujin/Slash");
		//ka->AddAnimation("idle", idleAnim);

		std::shared_ptr<Ideal::IMeshObject> mesh = Renderer->CreateStaticMeshObject(L"statue_chronos/statue_join");
		std::shared_ptr<Ideal::IMeshObject> mesh2 = Renderer->CreateStaticMeshObject(L"statue_chronos/statue_join");
		std::shared_ptr<Ideal::IMeshObject> mesh3 = Renderer->CreateStaticMeshObject(L"Tower/Tower");

		//-------------------Add Animation to Skinned Mesh Object-------------------//
		ka->AddAnimation("Run",runAnim);
		ka->SetAnimation("Run", true);
		cat->AddAnimation("Walk", walkAnim);

		//-------------------Add Mesh Object to Render Scene-------------------//
		//renderScene->AddObject(ka);
		//renderScene->AddObject(cat);
		//renderScene->AddObject(mesh);
		//renderScene->AddObject(mesh2);
		renderScene->AddObject(mesh3);

		DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
		float angle = 0.f;

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				angle += 0.2f;
				world = Matrix::CreateRotationY(DirectX::XMConvertToRadians(angle)) * Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
				world.CreateRotationY(angle);
				mesh2->SetTransformMatrix(world);
				//ka->SetTransformMatrix(world);
				//cat->SetTransformMatrix(world);
				// MAIN LOOP
				Renderer->Tick();
				Renderer->Render();
			}
		}
	}

#ifdef _DEBUG
	_ASSERT(_CrtCheckMemory());
#endif


	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IDEALGRAPHICSDEMO));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT windowRect = { 0,0, WIDTH, HEIGHT };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);
	g_hWnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 메뉴 선택을 구문 분석합니다:
			switch (wmId)
			{
				break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
