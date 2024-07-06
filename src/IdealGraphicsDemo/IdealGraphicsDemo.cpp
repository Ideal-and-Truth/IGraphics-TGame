﻿// IdealGraphicsDemo.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "IdealGraphicsDemo.h"

#ifdef _DEBUG
#pragma comment(lib, "DebugLib/GraphicsEngine/IdealGraphics.lib")
#else
#pragma comment(lib, "ReleaseLib/GraphicsEngine/IdealGraphics.lib")
#endif

//#ifdef _DEBUG
//#ifdef UNICODE
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
//#else
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif
//#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// D3D12 Agility SDK Runtime

//extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 613; }
//
//#if defined(_M_ARM64EC)
//extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\arm64\\"; }
//#elif defined(_M_ARM64)
//extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\arm64\\"; }
//#elif defined(_M_AMD64)
//extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\x64\\"; }
//#elif defined(_M_IX86)
//extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\x86\\"; }
//#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////


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

//#include "Editor/imgui/imgui.h"
#include "GraphicsEngine/public/imgui.h"

//#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
//#include "GraphicsEngine/public/ICamera.h"
#include "../Utils/SimpleMath.h"
#include "Test.h"

using namespace DirectX::SimpleMath;

#define MAX_LOADSTRING 100

#define WIDTH 1280
#define HEIGHT 960
HWND g_hWnd;
std::shared_ptr<Ideal::IdealRenderer> gRenderer = nullptr;
bool show_demo_window = true;
bool show_another_window = false;
bool show_angle_window = true;
bool show_point_light_window = true;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

DWORD g_FrameCount = 0;
ULONGLONG g_PrvFrameCheckTick = 0;
ULONGLONG g_PrvUpdateTick = 0;


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
void ImGuiTest();
void DirLightAngle(float* x, float* y, float* z);
void PointLightInspecter(std::shared_ptr<Ideal::IPointLight> light);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(428);
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
		bool isEditor = false;
		//EGraphicsInterfaceType type = EGraphicsInterfaceType::D3D12;
		EGraphicsInterfaceType type = EGraphicsInterfaceType::D3D12_RAYTRACING;
		if (isEditor)
		{
			type = EGraphicsInterfaceType::D3D12_EDITOR;
		}

		gRenderer = CreateRenderer(
			type,
			&g_hWnd,
			WIDTH,
			HEIGHT,
			L"../Resources/Assets/",
			L"../Resources/Models/",
			L"../Resources/Textures/"
		);

		gRenderer->Init();

		//gRenderer->SetSkyBox(L"../Resources/Textures/SkyBox/flower_road_8khdri_1kcubemap.BC7.DDS");
		gRenderer->SetSkyBox(L"../Resources/Textures/SkyBox/custom1.dds");

		Vector3 pointLightPosition = Vector3(0.f);

		//-------------------Create Camera-------------------//
		std::shared_ptr<Ideal::ICamera> camera = gRenderer->CreateCamera();
		InitCamera(camera);
		gRenderer->SetMainCamera(camera);

		//-------------------Create Scene-------------------//
		std::shared_ptr<Ideal::IRenderScene> renderScene = gRenderer->CreateRenderScene();
		gRenderer->SetRenderScene(renderScene);

		//-------------------Convert FBX(Model, Animation)-------------------//
		//ERROR : gRenderer->ConvertAnimationAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx"); -> Assimp Converter에서 FLAG 해제
		//gRenderer->ConvertAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx", true);
		//gRenderer->ConvertAssetToMyFormat(L"Kachujin/Mesh.fbx", true);
		//gRenderer->ConvertAssetToMyFormat(L"Boss/bosshall.fbx", false, false);
		//ReadVertexPosition(L"../Resources/Models/Tower/Tower.pos");
		//gRenderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Run.fbx");
		//gRenderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Idle.fbx");
		//gRenderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Slash.fbx");
		//gRenderer->ConvertAssetToMyFormat(L"statue_chronos/SMown_chronos_statue.fbx", false);

		//-------------------Test Vertices Pos-------------------//
		//ReadVertexPosition(L"../Resources/Models/Tower/Tower.pos");

		//-------------------Create Mesh Object-------------------//
		//std::shared_ptr<Ideal::ISkinnedMeshObject> ka = gRenderer->CreateSkinnedMeshObject(L"Kachujin/Mesh");
		//std::shared_ptr<Ideal::ISkinnedMeshObject> ka2 = gRenderer->CreateSkinnedMeshObject(L"Kachujin/Mesh");
		std::shared_ptr<Ideal::IAnimation> runAnim = gRenderer->CreateAnimation(L"Kachujin/Run");
		std::shared_ptr<Ideal::IAnimation> slashAnim = gRenderer->CreateAnimation(L"Kachujin/Slash");
		//std::shared_ptr<Ideal::ISkinnedMeshObject> cat = gRenderer->CreateSkinnedMeshObject(L"CatwalkWalkForward3/CatwalkWalkForward3");
		std::shared_ptr<Ideal::IAnimation> walkAnim = gRenderer->CreateAnimation(L"CatwalkWalkForward3/CatwalkWalkForward3");
		//
		////std::shared_ptr<Ideal::IAnimation> idleAnim = gRenderer->CreateAnimation(L"Kachujin/Idle");
		////std::shared_ptr<Ideal::IAnimation> slashAnim = gRenderer->CreateAnimation(L"Kachujin/Slash");
		////
		//std::shared_ptr<Ideal::IMeshObject> mesh = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");
		//std::shared_ptr<Ideal::IMeshObject> mesh = gRenderer->CreateStaticMeshObject(L"Boss/bosshall");
		//std::shared_ptr<Ideal::IMeshObject> mesh2 = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");
		//std::shared_ptr<Ideal::IMeshObject> mesh3 = gRenderer->CreateStaticMeshObject(L"Tower/Tower");
		////std::shared_ptr<Ideal::IMeshObject> mesh2 = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");
		//
		////-------------------Add Animation to Skinned Mesh Object-------------------//
		//ka->AddAnimation("Run", runAnim);
		//ka->SetAnimation("Run", true);
		//ka2->AddAnimation("Slash", slashAnim);
		//ka2->SetAnimation("Slash", true);
		//cat->AddAnimation("Walk", walkAnim);

		//-------------------Add Mesh Object to Render Scene-------------------//
		//renderScene->AddObject(ka);
		//renderScene->AddObject(cat);
		//renderScene->AddObject(mesh);
		//renderScene->AddDebugObject(mesh);
		//renderScene->AddObject(mesh2);

		static int transformX = 0.f;
		std::vector<std::shared_ptr<Ideal::IMeshObject>> meshes;
		{
			//for (int i = 0; i < 0; i++)
			//{
			//	//std::shared_ptr<Ideal::IMeshObject> mesh = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");
			//	//Matrix mat = Matrix::Identity;
			//	//mat.Translation(Vector3(i * 1.f, 0.f, 5.f));
			//	//mesh->SetTransformMatrix(mat);
			//	////renderScene->AddObject(mesh);
			//	//meshes.push_back(mesh);
			//	//
			//	std::shared_ptr<Ideal::ISkinnedMeshObject> ka = gRenderer->CreateSkinnedMeshObject(L"Kachujin/Mesh");
			//	if (i % 2 == 0)
			//		ka->AddAnimation("Run", runAnim);
			//	else
			//		ka->AddAnimation("Slash", slashAnim);
			//	
			//	Matrix mat2 = Matrix::Identity;
			//	mat2.Translation(Vector3(i * 1.f, 0.f, 0.f));
			//	//
			//	//meshes.push_back(mesh);
			//	ka->SetTransformMatrix(mat2);
			//	//
			//	//std::shared_ptr<Ideal::IMeshObject> mesh2 = gRenderer->CreateStaticMeshObject(L"Tower/Tower");
			//	//Matrix mat3 = Matrix::Identity;
			//	//mat3.Translation(Vector3(i * 1.f, 0.f, 15.f));
			//	//mesh2->SetTransformMatrix(mat3);
			//	//transformX += 1;
			//}
		}
		//renderScene->AddObject(mesh2);
		//renderScene->AddObject(mesh3);

		//--------------------Create Light----------------------//
		//std::shared_ptr<Ideal::IDirectionalLight> dirLight = gRenderer->CreateDirectionalLight();
		//std::shared_ptr<Ideal::ISpotLight> spotLight = gRenderer->CreateSpotLight();
		//std::shared_ptr<Ideal::IPointLight> pointLight = gRenderer->CreatePointLight();
		//std::shared_ptr<Ideal::IPointLight> pointLight2 = Renderer->CreatePointLight();

		//dirLight->SetDirection(Vector3(1.f, 0.f, 1.f));

		//pointLight->SetPosition(pointLightPosition);
		//pointLight->SetRange(300.f);
		//pointLight->SetLightColor(Color(1.f, 0.f, 1.f, 1.f));
		//pointLight->SetIntensity(10.f);

		//------------------Add Light to Render Scene-----------------//
		// Directional Light일 경우 그냥 바뀐다.
		//renderScene->AddLight(dirLight);
		//renderScene->AddLight(spotLight);
		//renderScene->AddLight(pointLight);
		//renderScene->AddLight(pointLight2);


		//mesh3->SetTransformMatrix(Matrix::CreateTranslation(Vector3(5.f, 0.f, 0.f)));
		//cat->SetTransformMatrix(Matrix::CreateTranslation(Vector3(2.f, 0.f, 0.f)));
		//ka2->SetTransformMatrix(Matrix::CreateTranslation(Vector3(-2.f, 0.f, 0.f)));
		//mesh2->SetTransformMatrix(Matrix::CreateTranslation(Vector3(-2.f, 0.f, 0.f)));

		DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
		DirectX::SimpleMath::Matrix world2 = DirectX::SimpleMath::Matrix::Identity;
		float angle = 0.f;
		float angleX = 0.f;
		float angleY = 0.f;
		float angleZ = 0.f;

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				g_FrameCount++;
				ULONGLONG curTick = GetTickCount64();
				if (curTick - g_PrvUpdateTick > 16)
				{
					g_PrvUpdateTick = curTick;
				}
				if (curTick - g_PrvFrameCheckTick > 1000)
				{
					g_PrvFrameCheckTick = curTick;
					WCHAR wchTxt[64];
					swprintf_s(wchTxt, L"FPS:%u", g_FrameCount);
					SetWindowText(g_hWnd, wchTxt);
					g_FrameCount = 0;
				}

				CameraTick(camera);
				//pointLight->SetPosition(camera->GetPosition());
				//auto cp = camera->GetPosition();
				//auto pp = pointLight->GetPosition();

				//angle += 0.4f;
				if (angle > 360.f)
				{
					angle = 0.f;
				}
				world = Matrix::CreateRotationY(DirectX::XMConvertToRadians(angle)) * Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));

				world2 = Matrix::CreateRotationX(DirectX::XMConvertToRadians(angleX))
					* Matrix::CreateRotationY(DirectX::XMConvertToRadians(angleY))
					* Matrix::CreateRotationZ(DirectX::XMConvertToRadians(angleZ))
					* Matrix::CreateTranslation(Vector3(0.f, 0.f, 0.f));

				//dirLight->SetDirection(world2.Forward());

				//world.CreateRotationY(angle);

				//mesh2->SetTransformMatrix(world);

				//----- Set Draw -----//
				static int tX = 0;
				if (GetAsyncKeyState('Z') & 0x8000)
				{
					//if (tX == 0)
					//for(int i = 0; i < 10; i++)
					{
						tX += 3;
						//cat->SetDrawObject(false);
						std::shared_ptr<Ideal::ISkinnedMeshObject> ka;
						ka = gRenderer->CreateSkinnedMeshObject(L"Kachujin/Mesh");
						if (tX % 2 == 0)
							ka->AddAnimation("Run", runAnim);
						else
							ka->AddAnimation("Slash", slashAnim);
						//ka->AddAnimation("Run", runAnim);

						Matrix mat2 = Matrix::Identity;
						mat2.Translation(Vector3(tX * 1.f, 0.f, 0.f));
						ka->SetTransformMatrix(mat2);

						meshes.push_back(ka);


						std::shared_ptr<Ideal::IMeshObject> mesh0 = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");
						mesh0->SetTransformMatrix(mat2);
						meshes.push_back(mesh0);

						std::shared_ptr<Ideal::ISkinnedMeshObject> mesh1 = gRenderer->CreateSkinnedMeshObject(L"CatwalkWalkForward3/CatwalkWalkForward3");
						mesh1->AddAnimation("Walk", walkAnim);
						mesh1->SetTransformMatrix(mat2);
						meshes.push_back(mesh1);
					}


					//{
					//	//cat->SetDrawObject(false);
					//	std::shared_ptr<Ideal::ISkinnedMeshObject> ka;
					//	ka = gRenderer->CreateSkinnedMeshObject(L"Kachujin/Mesh");
					//	if (tX % 2 == 0)
					//		ka->AddAnimation("Run", runAnim);
					//	else
					//		ka->AddAnimation("Slash", slashAnim);
					//	//ka->AddAnimation("Run", runAnim);
					//
					//	Matrix mat2 = Matrix::Identity;
					//	mat2.Translation(Vector3(tX * 1.f, 0.f, 0.f));
					//	ka->SetTransformMatrix(mat2);
					//
					//	meshes.push_back(ka);
					//}
				}



				if (GetAsyncKeyState('C') & 0x8000)
				{
					std::shared_ptr<Ideal::ISkinnedMeshObject> ka;
					ka = gRenderer->CreateSkinnedMeshObject(L"CatwalkWalkForward3/CatwalkWalkForward3");
					ka->AddAnimation("Walk", walkAnim);

					Matrix mat2 = Matrix::Identity;
					mat2.Translation(Vector3(tX * 1.f, 0.f, 0.f));

					meshes.push_back(ka);
					ka->SetTransformMatrix(mat2);
					tX++;


					//}
					//if (GetAsyncKeyState('X') & 0x8000)
					//{
					std::shared_ptr<Ideal::IMeshObject> mesh;
					//if (tX % 2 == 0)
					mesh = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");
					//else
					//	mesh = gRenderer->CreateStaticMeshObject(L"Tower/Tower");

					//Matrix mat2 = Matrix::Identity;
					//mat2.Translation(Vector3(tX * 1.f, 0.f, 0.f));

					meshes.push_back(mesh);
					mesh->SetTransformMatrix(mat2);
					tX++;
				}

				if (GetAsyncKeyState('X') & 0x8000)
				{
					std::shared_ptr<Ideal::IMeshObject> mesh;
					mesh = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");

					Matrix mat2 = Matrix::Identity;
					mat2.Translation(Vector3(tX * 1.f, 0.f, 0.f));

					meshes.push_back(mesh);
					mesh->SetTransformMatrix(mat2);
					tX++;
				}

				if (GetAsyncKeyState('M') & 0x8000)
				{
					for (int i = 0; i < 3; i++)
					{
						if (meshes.size())
						{
							auto back = meshes.back();
							meshes.pop_back();

							gRenderer->DeleteMeshObject(back);
							if (tX > 0)
								tX--;
						}
					}
				}
				//-----ImGui Test-----//
				gRenderer->ClearImGui();
				if (isEditor)
				{
					//ImGuiTest();
					//DirLightAngle(&angleX, &angleY, &angleZ);
					//PointLightInspecter(pointLight);
				}
				// MAIN LOOP
				//gRenderer->Tick();
				gRenderer->Render();
			}
		}
		for (int i = 0; i < meshes.size(); i++)
		{
			gRenderer->DeleteMeshObject(meshes[i]);
			meshes[i].reset();
		}
		meshes.clear();
		gRenderer.reset();
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
	if (gRenderer)
	{
		gRenderer->SetImGuiWin32WndProcHandler(hWnd, message, wParam, lParam);
	}
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
		break;
		case WM_SIZE:
		{
			if (gRenderer)
			{
				RECT rect;
				GetClientRect(g_hWnd, &rect);
				//AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
				DWORD width = rect.right - rect.left;
				DWORD height = rect.bottom - rect.top;
				gRenderer->Resize(width, height);
			}
		}
		break;
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
	//Camera->SetLensWithoutAspect(0.7f * 3.141592f, 1.f, 3000.f);
	Camera->SetPosition(Vector3(3.f, 3.f, -10.f));
}

void CameraTick(std::shared_ptr<Ideal::ICamera> Camera)
{
	float speed = 0.2f;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		speed *= 0.1f;
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
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		auto p = Camera->GetPosition();
		p.y += speed;
		Camera->SetPosition(p);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		auto p = Camera->GetPosition();
		p.y -= speed;
		Camera->SetPosition(p);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		Camera->RotateY(-speed);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		Camera->RotateY(speed);
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		Camera->Pitch(-speed);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		Camera->Pitch(speed);
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
void DirLightAngle(float* x, float* y, float* z)
{
	if (show_angle_window)
	{
		ImGui::Begin("Directional Light Angle Window");
		ImGui::SliderFloat("X", x, 0.0f, 360.0f);
		ImGui::SliderFloat("Y", y, 0.0f, 360.0f);
		ImGui::SliderFloat("Z", z, 0.0f, 360.0f);
		ImGui::End();
	}
}

void PointLightInspecter(std::shared_ptr<Ideal::IPointLight> light)
{
	ImVec4 lightColor;
	{
		lightColor.x = light->GetLightColor().R();
		lightColor.y = light->GetLightColor().G();
		lightColor.z = light->GetLightColor().B();
		lightColor.w = light->GetLightColor().A();
	}

	Vector3 lightPosition = light->GetPosition();
	if (show_point_light_window)
	{
		ImGui::Begin("Point Light Inspector");
		ImGui::ColorEdit3("Light Color", (float*)&lightColor);
		light->SetLightColor(Color(lightColor.x, lightColor.y, lightColor.z, lightColor.w));
		ImGui::InputFloat3("Position", &lightPosition.x);
		light->SetPosition(lightPosition);
		ImGui::SameLine();
		ImGui::End();
	}
}

void ImGuiTest()
{
	{
		ImGuiIO& io = ImGui::GetIO();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			//ImGui::Image()

			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}
}