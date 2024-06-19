#include "GraphicsManager.h"
#include "IRenderScene.h"
#include "ISkinnedMeshObject.h"
#include "imgui.h"

/// <summary>
/// �׷��� ������ �����ϴ� �Ŵ���
/// </summary>
Truth::GraphicsManager::GraphicsManager()
	: m_renderer(nullptr)
	, m_renderScene(nullptr)
	, m_aspect(1.0f)
{

}

/// <summary>
/// �Ҹ���
/// </summary>
Truth::GraphicsManager::~GraphicsManager()
{

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

	// ���Ŀ� ī�޶� �Ѱ� �� �þ߰�
	m_aspect = static_cast<float>(_wight) / static_cast<float>(_height);
}

void Truth::GraphicsManager::Finalize()
{
}

/// <summary>
/// ����
/// </summary>
void Truth::GraphicsManager::Render()
{
	m_renderer->Render();
}

/// <summary>
/// ���� �� ������Ʈ
/// </summary>
/// <param name="_mesh">�Ž� �������̽�</param>
void Truth::GraphicsManager::AddObject(std::shared_ptr<Ideal::IMeshObject> _mesh)
{
	m_renderScene->AddObject(_mesh);
}

/// <summary>
/// ������ ������Ʈ ����
/// </summary>
/// <param name="_mesh">�Ž� �������̽�</param>
void Truth::GraphicsManager::AddDebugobject(std::shared_ptr<Ideal::IMeshObject> _mesh)
{
	m_renderScene->AddDebugObject(_mesh);
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

/// <summary>
/// ī�޶� ����
/// </summary>
/// <returns>ī�޶� ������Ʈ</returns>
std::shared_ptr<Ideal::ICamera> Truth::GraphicsManager::CreateCamera()
{
	return m_renderer->CreateCamera();
}

/// <summary>
/// ���� ī�޶� ����
/// </summary>
/// <param name="_camera">ī�޶� ������Ʈ</param>
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

