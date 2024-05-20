// IdealGraphicsDemo.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "IdealGraphicsDemo.h"

#ifdef _DEBUG
#pragma comment(lib, "DebugLib/GraphicsEngine/IdealGraphics.lib")
#else
#pragma comment(lib, "ReleaseLib/GraphicsEngine/IdealGraphics.lib")
#endif

#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

#include <iostream>
using namespace std;

#include "GraphicsEngine/public/IdealRendererFactory.h"
#include "GraphicsEngine/public/IdealRenderer.h"
#include "GraphicsEngine/public/IMeshObject.h"
#include "GraphicsEngine/public/ISkinnedMeshObject.h"
#include "GraphicsEngine/public/IAnimation.h"
#include "GraphicsEngine/public/IRenderScene.h"
#include "GraphicsEngine/public/ICamera.h"

#include "GraphicsEngine/public/IDirectionalLight.h"
#include "GraphicsEngine/public/ISpotLight.h"
#include "GraphicsEngine/public/IPointLight.h"

//#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
//#include "GraphicsEngine/public/ICamera.h"
#include "../Utils/SimpleMath.h"
#include "Test.h"

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

// Test Function
void InitCamera(std::shared_ptr<Ideal::ICamera> Camera);
void CameraTick(std::shared_ptr<Ideal::ICamera> Camera);

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
		
		Vector3 pointLightPosition = Vector3(0.f);

		//-------------------Create Camera-------------------//
		std::shared_ptr<Ideal::ICamera> camera = Renderer->CreateCamera();
		InitCamera(camera);
		Renderer->SetMainCamera(camera);

		//-------------------Create Scene-------------------//
		std::shared_ptr<Ideal::IRenderScene> renderScene = Renderer->CreateRenderScene();
		Renderer->SetRenderScene(renderScene);

		//-------------------Convert FBX(Model, Animation)-------------------//
		//Renderer->ConvertAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx", true);
		//Renderer->ConvertAssetToMyFormat(L"Kachujin/Mesh.fbx", true);
		//Renderer->ConvertAssetToMyFormat(L"statue_chronos/statue_join.fbx", true);
		//Renderer->ConvertAssetToMyFormat(L"Tower/Tower.fbx", false, true);
		//Renderer->ConvertAnimationAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx");
		//Renderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Run.fbx");
		//Renderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Idle.fbx");
		//Renderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Slash.fbx");

		//-------------------Test Vertices Pos-------------------//
		//ReadVertexPosition(L"../Resources/Models/Tower/Tower.pos");

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
		renderScene->AddObject(cat);
		renderScene->AddObject(mesh);
		renderScene->AddObject(mesh2);
		renderScene->AddObject(mesh3);

		//--------------------Create Light----------------------//
		std::shared_ptr<Ideal::IDirectionalLight> dirLight = Renderer->CreateDirectionalLight();
		std::shared_ptr<Ideal::ISpotLight> spotLight = Renderer->CreateSpotLight();
		std::shared_ptr<Ideal::IPointLight> pointLight = Renderer->CreatePointLight();
		//std::shared_ptr<Ideal::IPointLight> pointLight2 = Renderer->CreatePointLight();

		dirLight->SetDirection(Vector3(1.f,0.f,1.f));

		pointLight->SetPosition(pointLightPosition);
		pointLight->SetRange(300.f);
		pointLight->SetLightColor(Color(1.f,0.f,1.f,1.f));
		pointLight->SetIntensity(10.f);
		
		/*pointLight2->SetPosition(pointLightPosition);
		pointLight2->SetRange(300.f);
		pointLight2->SetLightColor(Color(0.f, 1.f, 0.f, 1.f));
		pointLight2->SetIntensity(10.f);
		pointLight2->SetPosition(Vector3(0.f, 100.f, -200.f));*/

		//------------------Add Light to Render Scene-----------------//
		// Directional Light일 경우 그냥 바뀐다.
		renderScene->AddLight(dirLight);
		renderScene->AddLight(spotLight);
		renderScene->AddLight(pointLight);
		//renderScene->AddLight(pointLight2);


		mesh3->SetTransformMatrix(Matrix::CreateRotationX(DirectX::XMConvertToRadians(90.f)));

		DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
		DirectX::SimpleMath::Matrix world2 = DirectX::SimpleMath::Matrix::Identity;
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
				CameraTick(camera);
				pointLight->SetPosition(camera->GetPosition());
				auto cp = camera->GetPosition();
				auto pp = pointLight->GetPosition();
				//cout << "CameraPostion : " << cp.x << ", " << cp.y << ", " << cp.z << endl;// " | pointLight Position : " <<
				cout << "PointLightPosition : " << pp.x << ", " << pp.y << ", " << pp.z << endl;// " | pointLight Position : " <<
				//angle += 0.2f;
				angle += 0.4f;
				world = Matrix::CreateRotationY(DirectX::XMConvertToRadians(angle)) * Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
				world2 = Matrix::CreateRotationY(DirectX::XMConvertToRadians(angle)) * Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));
				
				dirLight->SetDirection(world2.Forward());

				world.CreateRotationY(angle);

				mesh2->SetTransformMatrix(world);
				//ka->SetTransformMatrix(world);
				//cat->SetTransformMatrix(world);
				if (GetAsyncKeyState('Z') & 0x8000)
				{
					cat->SetDrawObject(false);
				}
				if (GetAsyncKeyState('X') & 0x8000)
				{
					cat->SetDrawObject(true);
				}
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

void InitCamera(std::shared_ptr<Ideal::ICamera> Camera)
{
	float aspectRatio = float(WIDTH) / HEIGHT;
	Camera->SetLens(0.25f * 3.141592f, aspectRatio, 1.f, 3000.f);
	Camera->SetPosition(Vector3(0.f, 100.f, -200.f));
}

void CameraTick(std::shared_ptr<Ideal::ICamera> Camera)
{
	float speed = 2.f;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		speed = 0.2f;
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		Camera->Walk(speed);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		Camera->Walk(-speed);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		Camera->Strafe(-speed);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		Camera->Strafe(speed);
	}
	if (GetAsyncKeyState('L') & 0x8000)
	{
		Camera->SetLook(Vector3(0.f, 1.f, 1.f));
	}
	if (GetAsyncKeyState('K') & 0x8000)
	{
		Camera->SetLook(Vector3(0.f, 0.f, -1.f));
	}
	if (GetAsyncKeyState('J') & 0x8000)
	{
		Camera->SetLook(Vector3(0.f, 0.f, 1.f));
	}
}