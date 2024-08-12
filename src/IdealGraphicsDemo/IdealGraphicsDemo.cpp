// IdealGraphicsDemo.cpp : 애플리케이션에 대한 진입점을 정의합니다.
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

#include "GraphicsEngine/public/ITexture.h"
#include "GraphicsEngine/public/IMesh.h"
#include "GraphicsEngine/public/IMaterial.h"
#include "GraphicsEngine/public/IBone.h"
#include "GraphicsEngine/public/ISprite.h"

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
void CameraTick(std::shared_ptr<Ideal::ICamera> Camera, std::shared_ptr<Ideal::ISpotLight> SpotLight = nullptr);
void ImGuiTest();
void DirLightAngle(float* x, float* y, float* z);
void PointLightInspecter(std::shared_ptr<Ideal::IPointLight> light);
void SkinnedMeshObjectAnimationTest(std::shared_ptr<Ideal::ISkinnedMeshObject> SkinnedMeshObject);
void AnimationTest(std::shared_ptr<Ideal::IAnimation> Animation);
void LightTest(std::shared_ptr<Ideal::IDirectionalLight> DirLight);
void ImageTest(std::shared_ptr<Ideal::ITexture> Texture);
void SpotLightInspector(std::shared_ptr<Ideal::ISpotLight> PointLight);
void SkinnedMeshObjectBoneInfoTest(std::shared_ptr<Ideal::ISkinnedMeshObject> SkinnedMeshObject);
void SkinnedMeshObjectGetMeshTest(std::shared_ptr<Ideal::ISkinnedMeshObject> SkinnedMeshObject, std::shared_ptr<Ideal::IMaterial> Material, std::shared_ptr<Ideal::IMaterial> Material2 = nullptr, std::shared_ptr<Ideal::ITexture> Texture = nullptr, std::shared_ptr<Ideal::ITexture> Texture2 = nullptr);
void SpriteTest(std::shared_ptr<Ideal::ISprite> Sprite);
void RendererSizeTest();
float lightColor[3] = { 1.f, 1.f, 1.f };
float lightAngleX = 0.f;
float lightAngleY = 0.f;

float g_cameraSpeed = 0.04f;
bool g_CameraMove = true;
void CameraWindow(std::shared_ptr<Ideal::ICamera> Camera);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(3822);
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
#pragma region EditorInitialize
		//EGraphicsInterfaceType type = EGraphicsInterfaceType::D3D12;
		//EGraphicsInterfaceType type = EGraphicsInterfaceType::D3D12_EDITOR;
		EGraphicsInterfaceType type = EGraphicsInterfaceType::D3D12_RAYTRACING;
		//EGraphicsInterfaceType type = EGraphicsInterfaceType::D3D12_RAYTRACING_EDITOR;
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

		gRenderer->SetSkyBox(L"../Resources/Textures/SkyBox/flower_road_8khdri_1kcubemap.BC7.DDS");
		//gRenderer->SetSkyBox(L"../Resources/Textures/SkyBox/custom1.dds");

		Vector3 pointLightPosition = Vector3(3.f);

		//-------------------Create Camera-------------------//
		std::shared_ptr<Ideal::ICamera> camera = gRenderer->CreateCamera();
		InitCamera(camera);
		gRenderer->SetMainCamera(camera);

		gRenderer->SetDisplayResolutionOption(Ideal::Resolution::EDisplayResolutionOption::R_1920_1080);

#pragma endregion

#pragma region FBXConvert
		//-------------------Convert FBX(Model, Animation)-------------------//
		//gRenderer->ConvertAssetToMyFormat(L"PlayerRe/SM_chronos.Main_tPose.fbx", true);
		//gRenderer->ConvertAssetToMyFormat(L"PlayerRe/untitled.fbx", true);

		//gRenderer->ConvertAssetToMyFormat(L"cart/SM_cart.fbx", false);
		//gRenderer->ConvertAssetToMyFormat(L"building/building_dummy3_hanna.fbx", false);
		//gRenderer->ConvertAssetToMyFormat(L"UVSphere/UVSphere.fbx", false);
		//gRenderer->ConvertAssetToMyFormat(L"player/SK_Fencer_Lady_Nude@T-Pose.fbx", true);
		//gRenderer->ConvertAssetToMyFormat(L"DebugObject/debugCube.fbx", false);
		//gRenderer->ConvertAssetToMyFormat(L"Kachujin/Mesh.fbx", true);
		//gRenderer->ConvertAssetToMyFormat(L"statue_chronos/SMown_chronos_statue.fbx", false);
		//gRenderer->ConvertAssetToMyFormat(L"formula1/Formula 1 mesh.fbx", false);
		//gRenderer->ConvertAnimationAssetToMyFormat(L"player/Hip Hop Dancing.fbx");
		//gRenderer->ConvertAnimationAssetToMyFormat(L"Kachujin/HipHop.fbx");
		//gRenderer->ConvertAssetToMyFormat(L"player/SK_Fencer_Lady_Nude.fbx", true);
		//gRenderer->ConvertAnimationAssetToMyFormat(L"player/Dancing Twerk.fbx");
		//gRenderer->ConvertAssetToMyFormat(L"player2/myPlayer2.fbx", true);
		//gRenderer->ConvertAnimationAssetToMyFormat(L"player2/Capoeira.fbx");
		//ERROR : 
		//gRenderer->ConvertAnimationAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx");// -> Assimp Converter에서 FLAG 해제
		//gRenderer->ConvertAssetToMyFormat(L"CatwalkWalkForward3/CatwalkWalkForward3.fbx", true);
		//gRenderer->ConvertAssetToMyFormat(L"test2/run_.fbx", true);
		//gRenderer->ConvertAnimationAssetToMyFormat(L"test2/run_.fbx");
		//gRenderer->ConvertAssetToMyFormat(L"Boss/bosshall.fbx", false, false);
		//ReadVertexPosition(L"../Resources/Models/Tower/Tower.pos");
		//gRenderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Run.fbx");
		//gRenderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Idle.fbx");
		//gRenderer->ConvertAnimationAssetToMyFormat(L"Kachujin/Slash.fbx");
		//gRenderer->ConvertAnimationAssetToMyFormat(L"PlayerRe/Sword And Shield Slash.fbx");
		//gRenderer->ConvertAssetToMyFormat(L"boss/bosshall.fbx", false);
#pragma endregion

		//-------------------Test Vertices Pos-------------------//
		//ReadVertexPosition(L"../Resources/Models/Tower/Tower.pos");

#pragma region CreateMeshObjectAndAnimation
		//-------------------Create Mesh Object-------------------//
		std::shared_ptr<Ideal::ISkinnedMeshObject> playerRe = gRenderer->CreateSkinnedMeshObject(L"PlayerRe/SM_chronos.Main_tPose");
		std::shared_ptr<Ideal::IAnimation> swordAnim = gRenderer->CreateAnimation(L"PlayerRe/Sword And Shield Slash");
		//playerRe->AddAnimation("SwordAnim", swordAnim);
		//playerRe->SetAnimation("SwordAnim");
		//std::shared_ptr<Ideal::ISkinnedMeshObject> playerRe = gRenderer->CreateSkinnedMeshObject(L"PlayerRe/untitled");
		//std::shared_ptr<Ideal::ISkinnedMeshObject> player3 = gRenderer->CreateSkinnedMeshObject(L"player/myPlayer");
		//std::shared_ptr<Ideal::ISkinnedMeshObject> tempPlayer = gRenderer->CreateSkinnedMeshObject(L"statue_chronos/SMown_chronos_statue");
		//std::shared_ptr<Ideal::IAnimation> rumbaAnim = gRenderer->CreateAnimation(L"player/Rumba Dancing");
		//player3->AddAnimation("rumba", rumbaAnim);

		//std::shared_ptr<Ideal::ISkinnedMeshObject> player = gRenderer->CreateSkinnedMeshObject(L"player/SK_Fencer_Lady_Nude@T-Pose");
		//std::shared_ptr<Ideal::IAnimation> hiphopAnim = gRenderer->CreateAnimation(L"player/Hip Hop Dancing");
		//player->AddAnimation("Hip", hiphopAnim);
		//std::shared_ptr<Ideal::IAnimation> CapoeiraAnim = gRenderer->CreateAnimation(L"player/Capoeira");
		//player->AddAnimation("Hip", CapoeiraAnim);
		//std::shared_ptr<Ideal::ISkinnedMeshObject> player3 = gRenderer->CreateSkinnedMeshObject(L"player/SK_Fencer_Lady_Nude");
		//std::shared_ptr<Ideal::IAnimation> twerkAnim = gRenderer->CreateAnimation(L"player/Dancing Twerk");
		//player3->AddAnimation("Twerk",twerkAnim);
		//player->SetTransformMatrix(Matrix::CreateTranslation(Vector3(4,0,0)));

		//std::shared_ptr<Ideal::ISkinnedMeshObject> player2 = gRenderer->CreateSkinnedMeshObject(L"player2/myPlayer2");
		//std::shared_ptr<Ideal::IAnimation> rumba = gRenderer->CreateAnimation(L"player2/Capoeira");
		//player2->AddAnimation("Rumba", rumba);

		//std::shared_ptr<Ideal::ISkinnedMeshObject> ka = gRenderer->CreateSkinnedMeshObject(L"Kachujin/Mesh");
		//std::shared_ptr<Ideal::IAnimation> hiphopAnim2 = gRenderer->CreateAnimation(L"Kachujin/HipHop");
		//ka->AddAnimation("HIP", hiphopAnim2);
		//std::shared_ptr<Ideal::ISkinnedMeshObject> ka2 = gRenderer->CreateSkinnedMeshObject(L"Kachujin/Mesh");
		std::shared_ptr<Ideal::IAnimation> runAnim = gRenderer->CreateAnimation(L"Kachujin/Run");
		std::shared_ptr<Ideal::IAnimation> slashAnim = gRenderer->CreateAnimation(L"Kachujin/Slash");
		//std::shared_ptr<Ideal::ISkinnedMeshObject> cat = gRenderer->CreateSkinnedMeshObject(L"CatwalkWalkForward3/CatwalkWalkForward3");
		std::shared_ptr<Ideal::IAnimation> walkAnim = gRenderer->CreateAnimation(L"CatwalkWalkForward3/CatwalkWalkForward3");
		//
		////std::shared_ptr<Ideal::IAnimation> idleAnim = gRenderer->CreateAnimation(L"Kachujin/Idle");
		////std::shared_ptr<Ideal::IAnimation> slashAnim = gRenderer->CreateAnimation(L"Kachujin/Slash");
		////
		//std::shared_ptr<Ideal::IMeshObject> mesh = gRenderer->CreateStaticMeshObject(L"Boss/bosshall");
		//std::shared_ptr<Ideal::IMeshObject> mesh2 = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");
		//std::shared_ptr<Ideal::IMeshObject> mesh3 = gRenderer->CreateStaticMeshObject(L"Tower/Tower");
		////std::shared_ptr<Ideal::IMeshObject> mesh2 = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");
		//
		//std::shared_ptr<Ideal::IMeshObject> mesh = gRenderer->CreateStaticMeshObject(L"DebugObject/debugCube");
		//std::shared_ptr<Ideal::IMeshObject> mesh = gRenderer->CreateStaticMeshObject(L"cart/SM_cart");
		Matrix floorMat = Matrix::CreateRotationY(3.14);
		//mesh->SetTransformMatrix(floorMat);
		//std::shared_ptr<Ideal::IMeshObject> sphere = gRenderer->CreateStaticMeshObject(L"UVSphere/UVSphere");
		//std::shared_ptr<Ideal::IMeshObject> sphere1 = gRenderer->CreateStaticMeshObject(L"UVSphere1/UVSphere");
		//std::shared_ptr<Ideal::IMeshObject> sphere2 = gRenderer->CreateStaticMeshObject(L"UVSphere2/UVSphere");
		//std::shared_ptr<Ideal::IMeshObject> sphere3 = gRenderer->CreateStaticMeshObject(L"UVSphere3/UVSphere");
		//std::shared_ptr<Ideal::IMeshObject> sphere4 = gRenderer->CreateStaticMeshObject(L"UVSphere4/UVSphere");
		//std::shared_ptr<Ideal::IMeshObject> sphere5 = gRenderer->CreateStaticMeshObject(L"UVSphere5/UVSphere");
		//std::shared_ptr<Ideal::IMeshObject> sphere6 = gRenderer->CreateStaticMeshObject(L"UVSphere6/UVSphere");
		//std::shared_ptr<Ideal::IMeshObject> sphere7 = gRenderer->CreateStaticMeshObject(L"UVSphere7/UVSphere");
		//std::shared_ptr<Ideal::IMeshObject> sphere8 = gRenderer->CreateStaticMeshObject(L"UVSphere8/UVSphere");
		//sphere1->SetTransformMatrix(Matrix::CreateTranslation(Vector3(2.5f, 0.f, 0.f)));
		//sphere2->SetTransformMatrix(Matrix::CreateTranslation(Vector3(0.f, 2.5f, 0.f)));
		//sphere3->SetTransformMatrix(Matrix::CreateTranslation(Vector3(2.5f, 2.5f, 0.f)));
		//sphere4->SetTransformMatrix(Matrix::CreateTranslation(Vector3(0.f, 5.f, 0.f)));
		//sphere5->SetTransformMatrix(Matrix::CreateTranslation(Vector3(2.5f, 5.f, 0.f)));
		//sphere6->SetTransformMatrix(Matrix::CreateTranslation(Vector3(5.f, 0.f, 0.f)));
		//sphere7->SetTransformMatrix(Matrix::CreateTranslation(Vector3(5.f, 2.5f, 0.f)));
		//sphere8->SetTransformMatrix(Matrix::CreateTranslation(Vector3(5.f, 5.f, 0.f)));

		//std::shared_ptr<Ideal::IMeshObject> car = gRenderer->CreateStaticMeshObject(L"formula1/Formula 1 mesh");
		//std::shared_ptr<Ideal::IMeshObject> building = gRenderer->CreateStaticMeshObject(L"building/building_dummy3_hanna");
		//std::shared_ptr<Ideal::IMeshObject> boss = gRenderer->CreateStaticMeshObject(L"boss/bosshall");

		////-------------------Add Animation to Skinned Mesh Object-------------------//
		//ka->AddAnimation("Run", runAnim);
		//ka->AddAnimation("Slash", slashAnim);
		//ka->SetAnimation("Run", true);
		//ka->AddAnimation("HipHop", hiphopAnim2);
		//cat->AddAnimation("Walk", walkAnim);

		std::vector<std::shared_ptr<Ideal::IMeshObject>> meshes;

#pragma endregion

#pragma region CreateTextureAndMaterial
		//--------------------Create Texture----------------------//
		std::shared_ptr<Ideal::ITexture> faceTexture = gRenderer->CreateTexture(L"../Resources/Textures/PlayerRe/T_face_BaseMap.png");
		std::shared_ptr<Ideal::ITexture> faceNormalTexture = gRenderer->CreateTexture(L"../Resources/Textures/PlayerRe/T_face_Normal.png");
		std::shared_ptr<Ideal::ITexture> skirtBottomTexture = gRenderer->CreateTexture(L"../Resources/Textures/PlayerRe/T_skirtbottom_BaseMap.png");
		std::shared_ptr<Ideal::ITexture> skirtBottomNormalTexture = gRenderer->CreateTexture(L"../Resources/Textures/PlayerRe/T_skirtbottom_Normal.png");
		std::shared_ptr<Ideal::ITexture> eyeTexture = gRenderer->CreateTexture(L"../Resources/Textures/PlayerRe/T_eyes_BaseMap.png");
		//testTexture2 = nullptr;
		//std::shared_ptr<Ideal::ITexture> testTexture = nullptr;

		//--------------------Create Material----------------------//
		std::shared_ptr<Ideal::IMaterial> skirtMaterial = gRenderer->CreateMaterial();
		skirtMaterial->SetBaseMap(skirtBottomTexture);
		skirtMaterial->SetNormalMap(skirtBottomNormalTexture);

		std::shared_ptr<Ideal::IMaterial> eyeMaterial = gRenderer->CreateMaterial();
		eyeMaterial->SetBaseMap(eyeTexture);
#pragma endregion

#pragma region CreateSpriteUI
		std::shared_ptr<Ideal::ISprite> sprite = gRenderer->CreateSprite();
		sprite->SetTexture(faceTexture);
		//sprite->SetTextureSamplePosition(Vector2(0, 0));
		sprite->SetScale(Vector2(0.1, 0.1));
		sprite->SetPosition(Vector2(0, 0));
		sprite->SetAlpha(0.8f);
		sprite->SetZ(0.2);
		// 아래의 값은 기본으로 적용되어 있음. (Set Texture 할 때 Texture의 사이즈로 아래의 작업을 함)
		sprite->SetSampleRect({ 0,0,faceTexture->GetWidth(), faceTexture->GetHeight()});	
		
		//sprite->SetTextureSize(Vector2(512, 512));
		//sprite->SetTextureSamplePosition(Vector2(0, 0));
		//sprite->SetTextureSampleSize(Vector2(2048, 2048));

		std::shared_ptr<Ideal::ISprite> sprite2 = gRenderer->CreateSprite();
		sprite2->SetTexture(skirtBottomTexture);
		sprite2->SetScale(Vector2(0.1, 0.1));
		sprite2->SetPosition(Vector2(50, 50));
		sprite2->SetZ(0.1);

		std::shared_ptr<Ideal::ISprite> sprite3 = gRenderer->CreateSprite();
		sprite3->SetScale(Vector2(0.3, 0.3));
		sprite3->SetPosition(Vector2(200, 0));
		sprite3->SetZ(0);
		

#pragma endregion

#pragma region CreateLight
		//--------------------Create Light----------------------//
		std::shared_ptr<Ideal::IDirectionalLight> dirLight = gRenderer->CreateDirectionalLight();
		dirLight->SetDirection(Vector3(1.f, 0.f, 0.f));

		//std::shared_ptr<Ideal::IPointLight> pointLight2 = Renderer->CreatePointLight();

		std::shared_ptr<Ideal::ISpotLight> spotLight = gRenderer->CreateSpotLight();
		spotLight->SetPosition(Vector3(0.f, 3.f, 3.f));
		spotLight->SetRange(6.f);
		spotLight->SetLightColor(Color(1.f, 0.f, 1.f, 1.f));
		spotLight->SetIntensity(0.f);


		std::shared_ptr<Ideal::IPointLight> pointLight = gRenderer->CreatePointLight();
		pointLight->SetPosition(Vector3(0.f, 3.f, 3.f));
		pointLight->SetRange(6.f);
		pointLight->SetLightColor(Color(1.f, 0.f, 1.f, 1.f));
		pointLight->SetIntensity(0.f);

#pragma endregion

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

				CameraTick(camera, spotLight);
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
					if (sprite)
						gRenderer->DeleteSprite(sprite);
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

					std::shared_ptr<Ideal::IMeshObject> mesh;
					mesh = gRenderer->CreateStaticMeshObject(L"statue_chronos/SMown_chronos_statue");
					
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
				// Animation // 역재생 안됨
				//ka->AnimationDeltaTime(0.002f);
				//cat->AnimationDeltaTime(0.002f);
				//player->AnimationDeltaTime(0.002f);
				//player3->AnimationDeltaTime(0.002f);
				playerRe->AnimationDeltaTime(0.002f);
				//-----ImGui Test-----//
				gRenderer->ClearImGui();
				//if (isEditor)
				if (type == EGraphicsInterfaceType::D3D12_EDITOR || type == EGraphicsInterfaceType::D3D12_RAYTRACING_EDITOR)
				{
					//static int once = 0;
					//if (once != 0)
					{
						RendererSizeTest();
						CameraWindow(camera);
						AnimationTest(slashAnim);
						//SkinnedMeshObjectAnimationTest(ka);
						if (dirLight)
						{
							LightTest(dirLight);
						}
						if (pointLight)
						{
							PointLightInspecter(pointLight);
						}
						if (spotLight)
						{
							SpotLightInspector(spotLight);
						}
						if (faceTexture)
						{
							ImageTest(faceTexture);
						}
						if (eyeTexture)
						{
							ImageTest(eyeTexture);
						}
						if (skirtBottomTexture)
						{
							ImageTest(skirtBottomTexture);
						}
						if (playerRe)
						{
							SkinnedMeshObjectBoneInfoTest(playerRe);
							SkinnedMeshObjectGetMeshTest(playerRe, skirtMaterial, eyeMaterial, faceTexture, faceNormalTexture);
						}
						//if (sprite)
						//{
						//	SpriteTest(sprite);
						//}
						if (sprite3)
						{
							SpriteTest(sprite3);
						}
					}
					//once++;
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

		//gRenderer->DeleteMeshObject(mesh);
		//mesh.reset();
		//
		//gRenderer->DeleteMeshObject(cat);
		//cat.reset();

		//gRenderer->DeleteMeshObject(car);
		//car.reset();

		//gRenderer->DeleteMeshObject(boss);
		//boss.reset();
		gRenderer->DeleteMeshObject(playerRe);
		playerRe.reset();
		gRenderer->DeleteMaterial(skirtMaterial);
		skirtMaterial.reset();
		gRenderer->DeleteMaterial(eyeMaterial);
		eyeMaterial.reset();
		gRenderer->DeleteTexture(faceTexture);
		faceTexture.reset();
		gRenderer->DeleteTexture(faceNormalTexture);
		faceNormalTexture.reset();
		gRenderer->DeleteTexture(eyeTexture);
		eyeTexture.reset();
		gRenderer->DeleteTexture(skirtBottomTexture);
		skirtBottomTexture.reset();
		gRenderer->DeleteTexture(skirtBottomNormalTexture);
		skirtBottomNormalTexture.reset();
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
			break;
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
	//float aspectRatio = float(1296) / 999.f;
	//Camera->SetLens(0.25f * 3.141592f, aspectRatio, 1.f, 3000.f);
	Camera->SetLens(0.25f * 3.141592f, aspectRatio, 1.f, 10000.f);
	//Camera->SetLensWithoutAspect(0.7f * 3.141592f, 1.f, 3000.f);
	Camera->SetPosition(Vector3(3.f, 3.f, -10.f));
}

void CameraTick(std::shared_ptr<Ideal::ICamera> Camera, std::shared_ptr<Ideal::ISpotLight> SpotLight /*= nullptr*/)
{
	if (!g_CameraMove)
		return;
	float speed = g_cameraSpeed;
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
		Camera->RotateY(-speed * 0.2);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		Camera->RotateY(speed * 0.2);
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		Camera->Pitch(-speed * 0.2);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		Camera->Pitch(speed * 0.2);
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

	if (SpotLight)
	{
		SpotLight->SetPosition(Camera->GetPosition());
		SpotLight->SetDirection(Camera->GetLook());
	}
}

void CameraWindow(std::shared_ptr<Ideal::ICamera> Camera)
{
	if (show_angle_window)
	{
		ImGui::Begin("Camera Window");
		ImGui::DragFloat("Camera Speed", &g_cameraSpeed, 0.01f, 0.0f, 1.f);
		ImGui::Checkbox("Move", &g_CameraMove);
		ImGui::End();
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
	float range = light->GetRange();
	float intensity = light->GetIntensity();
	if (show_point_light_window)
	{
		ImGui::Begin("Point Light Inspector");
		ImGui::ColorEdit3("Light Color", (float*)&lightColor);
		light->SetLightColor(Color(lightColor.x, lightColor.y, lightColor.z, lightColor.w));
		//ImGui::InputFloat3("Position", &lightPosition.x);
		ImGui::DragFloat3("Position", &lightPosition.x, 1.f, 0.f, 10.f);
		light->SetPosition(lightPosition);

		ImGui::DragFloat("Range", &range, 1.f, 0.f, 1000.f);
		light->SetRange(range);

		ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.f, 100.f);
		light->SetIntensity(intensity);

		ImGui::End();
	}
}


void SpotLightInspector(std::shared_ptr<Ideal::ISpotLight> SpotLight)
{

	ImVec4 lightColor;
	{
		lightColor.x = SpotLight->GetLightColor().R();
		lightColor.y = SpotLight->GetLightColor().G();
		lightColor.z = SpotLight->GetLightColor().B();
		lightColor.w = SpotLight->GetLightColor().A();
	}

	ImVec4 lightPos;
	{
		lightPos.x = SpotLight->GetPosition().x;
		lightPos.y = SpotLight->GetPosition().y;
		lightPos.z = SpotLight->GetPosition().z;
	}

	float dir[3] = {
		SpotLight->GetDirection().x,
		SpotLight->GetDirection().y,
		SpotLight->GetDirection().z
	};

	float range = SpotLight->GetRange();
	float intensity = SpotLight->GetIntensity();
	float angle = SpotLight->GetSpotAngle();

	float softness = SpotLight->GetSoftness();

	ImGui::Begin("Spot Light Inspector");

	ImGui::InputFloat3("Light Position", &lightPos.x);
	ImGui::InputFloat3("Light Direction", &dir[0]);

	ImGui::ColorEdit3("Light Color", (float*)&lightColor);
	SpotLight->SetLightColor(Color(lightColor.x, lightColor.y, lightColor.z, lightColor.w));
	//ImGui::InputFloat3("Position", &lightPosition.x);
	ImGui::DragFloat("Angle", &angle, 1.f, 0.f, 10.f);
	SpotLight->SetSpotAngle(angle);

	ImGui::DragFloat("Range", &range, 1.f, 0.f, 1000.f);
	SpotLight->SetRange(range);

	ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.f, 100.f);
	SpotLight->SetIntensity(intensity);

	ImGui::DragFloat("Softness", &softness, 0.1f, 0.f, 30.f);
	SpotLight->SetSoftness(softness);

	ImGui::End();
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

void SkinnedMeshObjectAnimationTest(std::shared_ptr<Ideal::ISkinnedMeshObject> SkinnedMeshObject)
{
	ImGui::Begin("Skinned Mesh Object Animation Window");

	static float animationSpeed = 1.f; // 임시 static // 실제 사용 조심
	ImGui::SliderFloat("Animation Speed", &animationSpeed, 0.0f, 3.0f);
	SkinnedMeshObject->SetAnimationSpeed(animationSpeed);

	ImGui::Text("Current Animation Index : %d", SkinnedMeshObject->GetCurrentAnimationIndex());

	ImGui::Text("Change Next Animation");
	if (ImGui::Button("Run"))
	{
		SkinnedMeshObject->SetAnimation("Run", false);
	}
	if (ImGui::Button("Slash"))
	{
		SkinnedMeshObject->SetAnimation("Slash", false);
	}

	ImGui::Text("Reserve Next Animation");
	if (ImGui::Button("Run2"))
	{
		SkinnedMeshObject->SetAnimation("Run", true);
	}
	if (ImGui::Button("Slash2"))
	{
		SkinnedMeshObject->SetAnimation("Slash", true);
	}

	ImGui::End();
}

void AnimationTest(std::shared_ptr<Ideal::IAnimation> Animation)
{
	ImGui::Begin("Animation Window");
	ImGui::Text("Animation Max Frame Count : %d", Animation->GetFrameCount());
	ImGui::End();
}

void LightTest(std::shared_ptr<Ideal::IDirectionalLight> DirLight)
{
	ImGui::Begin("Directional Light");
	ImGui::Text("Rotation Axis X");
	ImGui::SliderFloat("X", &lightAngleX, 0.f, 6.28f);
	Matrix mat = Matrix::Identity;
	mat *= Matrix::CreateRotationX(lightAngleX);
	ImGui::ColorEdit3("Diffuse Color", lightColor);
	ImGui::SliderFloat("Y", &lightAngleY, 0.f, 6.28f);
	mat *= Matrix::CreateRotationY(lightAngleY);

	Vector3 rot = mat.Forward();
	if (DirLight)
	{
		DirLight->SetDirection(rot);
		DirLight->SetDiffuseColor(Color(lightColor[0], lightColor[1], lightColor[2], 1.f));
	}

	ImGui::End();
}

// ImGui에 이미지를 띄워보는 용도의 함수
void ImageTest(std::shared_ptr<Ideal::ITexture> Texture)
{
	ImGui::Begin("Image Test");
	const ImVec2 size(100, 100);
	ImGui::Image((ImTextureID)Texture->GetImageID(), size);
	ImGui::End();
}

// 기능 본 정보 추출
void SkinnedMeshObjectBoneInfoTest(std::shared_ptr<Ideal::ISkinnedMeshObject> SkinnedMeshObject)
{
	ImGui::Begin("SkinnedMesh Bone Test");
	auto boneSize = SkinnedMeshObject->GetBonesSize();
	ImGui::Separator();
	// 본 정보
	ImGui::Text("Bone Info");
	for (int i = 0; i < boneSize; ++i)
	{
		auto bone = SkinnedMeshObject->GetBoneByIndex(i);
		ImGui::Text(bone.lock()->GetName().c_str());
		auto matrix = bone.lock()->GetTransform();
	}
	ImGui::End();
}
// 기능 정리 :
// 모델에서 매쉬 오브젝트 추출 -> 매쉬 이거 가지고 있지 말 것. 지금 shared_ptr로 되어 있음
// 머테리얼 적용 및 가지고 있는 머테리얼 가져오기
void SkinnedMeshObjectGetMeshTest(std::shared_ptr<Ideal::ISkinnedMeshObject> SkinnedMeshObject, std::shared_ptr<Ideal::IMaterial> Material, std::shared_ptr<Ideal::IMaterial> Material2 /*= nullptr*/, std::shared_ptr<Ideal::ITexture> Texture /*= nullptr*/, std::shared_ptr<Ideal::ITexture> Texture2 /*= nullptr*/)
{
	ImGui::Begin("SkinnedMesh Get Mesh Test");
	auto meshSize = SkinnedMeshObject->GetMeshesSize();
	ImGui::Separator();

	//ImVec2 s(100, 100);
	//ImGui::Image(ImTextureID(SkinnedMeshObject->GetMeshByIndex(5).lock()->GetMaterialObject().lock()->GetBaseMap().lock()->GetImageID()), s);


	// 매쉬 정보
	ImGui::Text("Mesh Info");
	for (int i = 0; i < meshSize; ++i)
	{
		auto mesh = SkinnedMeshObject->GetMeshByIndex(i);
		ImGui::Text(mesh.lock()->GetName().c_str());
	}
	if (Texture)
	{
		static int once = 0;
		once++;
		if (once >= 3000)
		{
			once = 1;
			// 예시로 GetMaterialObject를 사용함. -> 이는 기본 머테리얼을 가져와서 사용하므로 가급적 사용X
			// 차라리 Material을 새로 만들어서 SetMaterialObject를 해주셈
			// 이건 그냥 SetBaseMap의 예시
			SkinnedMeshObject->GetMeshByIndex(5).lock()->GetMaterialObject().lock()->SetBaseMap(Texture);
			SkinnedMeshObject->GetMeshByIndex(5).lock()->GetMaterialObject().lock()->SetNormalMap(Texture2);
			int a = 3;
		}
	}

	// 아래는 머테리얼을 바꾼다
	if (Material)
	{
		static int once = 0;
		once++;
		if (once >= 1000)
		{
			once = 1;
			SkinnedMeshObject->GetMeshByIndex(0).lock()->SetMaterialObject(Material);
		}
	}
	if (Material2)
	{
		static int once = 0;
		once++;
		if (once >= 1000)
		{
			once = 1;
			SkinnedMeshObject->GetMeshByIndex(4).lock()->SetMaterialObject(Material2);
		}
	}
	ImGui::End();
}

void SpriteTest(std::shared_ptr<Ideal::ISprite> Sprite)
{
	ImGui::Begin("Sprite Test");
	float a = Sprite->GetAlpha();
	ImGui::SliderFloat("Alpha", &a, 0.f, 1.f);
	Sprite->SetAlpha(a);

	float c[4];
	c[0] = Sprite->GetColor().R();
	c[1] = Sprite->GetColor().G();
	c[2] = Sprite->GetColor().B();
	c[3] = 1;
	ImGui::ColorEdit3("Sprite Color", c);
	Sprite->SetColor(Color(c[0], c[1], c[2], c[3]));

	bool b = Sprite->GetActive();
	ImGui::Checkbox("Active", &b);
	Sprite->SetActive(b);

	ImGui::End();
}

void RendererSizeTest()
{
	ImGui::Begin("Window Size Test", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav);
	bool b = gRenderer->IsFullScreen();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("FullScreenMode").x - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.x);
	ImGui::Checkbox("FullScreenMode", &b);
	if (b != gRenderer->IsFullScreen())
	{
		gRenderer->ToggleFullScreenWindow();
	}

	static int val = 0;
	static const char* items[] =
	{
		"800x600",
		"1200x900",
		"1280x720",
		"1920x1080",
		"1920x1200",
		"2560x1440",
		"3440x1440",
		"3840x2160"
	};
	ImGui::Combo("Display Resolution", &val, items, IM_ARRAYSIZE(items));
	gRenderer->SetDisplayResolutionOption((Ideal::Resolution::EDisplayResolutionOption)val);
	ImGui::End();
}