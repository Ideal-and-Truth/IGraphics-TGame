#include "GraphicsManager.h"
#include "IRenderScene.h"
#include "ISkinnedMeshObject.h"
#include "imgui.h"

/// <summary>
/// 그래픽 엔진과 소통하는 매니저
/// </summary>
Truth::GraphicsManager::GraphicsManager()
	: m_renderer(nullptr)
	, m_renderScene(nullptr)
	, m_aspect(1.0f)
{

}

/// <summary>
/// 소멸자
/// </summary>
Truth::GraphicsManager::~GraphicsManager()
{

}

/// <summary>
/// 초기화
/// </summary>
/// <param name="_hwnd">윈도우 핸들러</param>
/// <param name="_wight">스크린 넓이</param>
/// <param name="_height">스크린 높이</param>
void Truth::GraphicsManager::Initalize(HWND _hwnd, uint32 _wight, uint32 _height)
{
	// Editor mode & Release mode
#ifdef _DEBUG
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12_EDITOR,
		&_hwnd,
		_wight,
		_height,
		m_assetPath[0],
		m_assetPath[1],
		m_assetPath[2]
	);
#else
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12,
		&_hwnd,
		_wight,
		_height,
		m_assetPath[0],
		m_assetPath[1],
		m_assetPath[2]
	);
#endif // _DEBUG

	
	m_renderer->Init();
	m_renderScene = m_renderer->CreateRenderScene();
	m_renderer->SetRenderScene(m_renderScene);

	// 추후에 카메라에 넘겨 줄 시야각
	m_aspect = static_cast<float>(_wight) / static_cast<float>(_height);
}

void Truth::GraphicsManager::Finalize()
{
}

/// <summary>
/// 렌더
/// </summary>
void Truth::GraphicsManager::Render()
{
	m_renderer->Render();
}

/// <summary>
/// 렌더 할 오브젝트
/// </summary>
/// <param name="_mesh">매쉬 인터페이스</param>
void Truth::GraphicsManager::AddObject(std::shared_ptr<Ideal::IMeshObject> _mesh)
{
	m_renderScene->AddObject(_mesh);
}

/// <summary>
/// 디버깅용 오브젝트 렌더
/// </summary>
/// <param name="_mesh">매쉬 인터페이스</param>
void Truth::GraphicsManager::AddDebugobject(std::shared_ptr<Ideal::IMeshObject> _mesh)
{
	m_renderScene->AddDebugObject(_mesh);
}

/// <summary>
/// Bone이 있는 스키닝 매쉬
/// </summary>
/// <param name="_path">파일 경로</param>
/// <returns>매쉬 오브젝트</returns>
std::shared_ptr<Ideal::ISkinnedMeshObject> Truth::GraphicsManager::CreateSkinnedMesh(std::wstring _path)
{
	return m_renderer->CreateSkinnedMeshObject(_path);
}

/// <summary>
/// 정적 오브젝트
/// </summary>
/// <param name="_path">파일 경로</param>
/// <returns>매쉬 오브젝트 </returns>
std::shared_ptr<Ideal::IMeshObject> Truth::GraphicsManager::CreateMesh(std::wstring _path)
{
	return m_renderer->CreateStaticMeshObject(_path);
}

/// <summary>
/// 카메라 생성
/// </summary>
/// <returns>카메라 오브젝트</returns>
std::shared_ptr<Ideal::ICamera> Truth::GraphicsManager::CreateCamera()
{
	return m_renderer->CreateCamera();
}

/// <summary>
/// 메인 카메라 지정
/// </summary>
/// <param name="_camera">카메라 오브젝트</param>
void Truth::GraphicsManager::SetMainCamera(std::shared_ptr<Ideal::ICamera> _camera)
{
	m_renderer->SetMainCamera(_camera);
}

void Truth::GraphicsManager::ResetRenderScene()
{
	m_renderScene.reset();
	m_renderScene = m_renderer->CreateRenderScene();
	m_renderer->SetRenderScene(m_renderScene);
}

