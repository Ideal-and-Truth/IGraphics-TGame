#include "GraphicsManager.h"
#include "IRenderScene.h"
#include "ISkinnedMeshObject.h"
#include "imgui.h"
#include "Camera.h"

#ifdef _DEBUG
#include "EditorCamera.h"
#endif // _DEBUG


/// <summary>
/// �׷��� ������ �����ϴ� �Ŵ���
/// </summary>
Truth::GraphicsManager::GraphicsManager()
	: m_renderer(nullptr)
	, m_aspect(1.0f)
	, m_mainCamera(nullptr)
{

}

/// <summary>
/// �Ҹ���
/// </summary>
Truth::GraphicsManager::~GraphicsManager()
{
	DEBUG_PRINT("Finalize GraphicsManager\n");
}

/// <summary>
/// �ʱ�ȭ
/// </summary>
/// <param name="_hwnd">������ �ڵ鷯</param>
/// <param name="_wight">��ũ�� ����</param>
/// <param name="_height">��ũ�� ����</param>
void Truth::GraphicsManager::Initalize(HWND _hwnd, uint32 _wight, uint32 _height)
{
	// Editor mode & Release mode
#ifdef _DEBUG
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12_RAYTRACING_EDITOR,
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

	// ���Ŀ� ī�޶� �Ѱ� �� �þ߰�
	m_aspect = static_cast<float>(_wight) / static_cast<float>(_height);

	m_renderer->SetSkyBox(L"../Resources/Textures/SkyBox/custom1.dds");
}

void Truth::GraphicsManager::Finalize()
{
	
}

/// <summary>
/// ����
/// </summary>
void Truth::GraphicsManager::Render()
{
#ifdef _DEBUG
	m_renderer->Render();
#else
	CompleteCamera();
	m_renderer->Render();
#endif // _DEBUG
}


void Truth::GraphicsManager::ConvertAsset(std::wstring _path, bool _isSkind /*= false*/, bool _isData /*= false*/, bool _isCenter)
{
	m_renderer->ConvertAssetToMyFormat(_path, _isSkind, _isData, _isCenter);
}

/// <summary>
/// Bone�� �ִ� ��Ű�� �Ž�
/// </summary>
/// <param name="_path">���� ���</param>
/// <returns>�Ž� ������Ʈ</returns>
std::shared_ptr<Ideal::ISkinnedMeshObject> Truth::GraphicsManager::CreateSkinnedMesh(std::wstring _path)
{
	return m_renderer->CreateSkinnedMeshObject(_path);
}

/// <summary>
/// ���� ������Ʈ
/// </summary>
/// <param name="_path">���� ���</param>
/// <returns>�Ž� ������Ʈ </returns>
std::shared_ptr<Ideal::IMeshObject> Truth::GraphicsManager::CreateMesh(std::wstring _path)
{
	return m_renderer->CreateStaticMeshObject(_path);
}

std::shared_ptr<Ideal::IMeshObject> Truth::GraphicsManager::CreateDebugMeshObject(std::wstring _path)
{
	return m_renderer->CreateDebugMeshObject(_path);
}

std::shared_ptr<Ideal::IAnimation> Truth::GraphicsManager::CreateAnimation(std::wstring _path)
{
	return m_renderer->CreateAnimation(_path);
}

std::shared_ptr<Ideal::IDirectionalLight> Truth::GraphicsManager::CreateDirectionalLight()
{
	return m_renderer->CreateDirectionalLight();
}

std::shared_ptr<Ideal::ISpotLight> Truth::GraphicsManager::CreateSpotLight()
{
	return m_renderer->CreateSpotLight();
}

std::shared_ptr<Ideal::IPointLight> Truth::GraphicsManager::CreatePointLight()
{
	return m_renderer->CreatePointLight();
}

/// <summary>
/// ī�޶� ����
/// </summary>
/// <returns>ī�޶� ������Ʈ</returns>
std::shared_ptr<Ideal::ICamera> Truth::GraphicsManager::CreateCamera()
{
	return m_renderer->CreateCamera();
}

/// <summary>
/// ��ϵ� Mesh
/// </summary>
/// <param name="_meshObject"></param>
void Truth::GraphicsManager::DeleteMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject)
{
	m_renderer->DeleteMeshObject(_meshObject);
}

void Truth::GraphicsManager::DeleteDebugMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject)
{
	m_renderer->DeleteDebugMeshObject(_meshObject);
}

/// <summary>
/// ���� ī�޶� ����
/// </summary>
/// <param name="_camera">ī�޶� ������Ʈ</param>
void Truth::GraphicsManager::SetMainCamera(Camera* _camera)
{
	m_renderer->SetMainCamera(_camera->m_camera);
	m_mainCamera = _camera;
}

#ifdef _DEBUG
void Truth::GraphicsManager::SetMainCamera(EditorCamera* _camera)
{
	m_renderer->SetMainCamera(_camera->m_camera);
	// m_mainCamera = _camera;
}
#endif // _DEBUG

void Truth::GraphicsManager::CompleteCamera()
{
	m_mainCamera->CompleteCamera();
}

