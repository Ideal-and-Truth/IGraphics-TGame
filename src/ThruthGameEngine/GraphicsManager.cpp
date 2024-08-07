#include "GraphicsManager.h"
#include "IRenderScene.h"
#include "ISkinnedMeshObject.h"
#include "imgui.h"
#include "Camera.h"
#include "Texture.h"
#include "Matarial.h"
#ifdef EDITOR_MODE
#include "EditorCamera.h"
#endif // EDITOR_MODE


/// <summary>
/// 그래픽 엔진과 소통하는 매니저
/// </summary>
Truth::GraphicsManager::GraphicsManager()
	: m_renderer(nullptr)
	, m_aspect(1.0f)
	, m_mainCamera(nullptr)
{

}

/// <summary>
/// 소멸자
/// </summary>
Truth::GraphicsManager::~GraphicsManager()
{
	DEBUG_PRINT("Finalize GraphicsManager\n");
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
#ifdef EDITOR_MODE
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12_RAYTRACING_EDITOR,
		//EGraphicsInterfaceType::D3D12_EDITOR,
		&_hwnd,
		_wight,
		_height,
		m_assetPath[0],
		m_assetPath[1],
		m_assetPath[2]
	);

#else
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12_RAYTRACING,
		&_hwnd,
		_wight,
		_height,
		m_assetPath[0],
		m_assetPath[1],
		m_assetPath[2]
	);
#endif // EDITOR_MODE


	m_renderer->Init();

	// 추후에 카메라에 넘겨 줄 시야각
	m_aspect = static_cast<float>(_wight) / static_cast<float>(_height);

	m_renderer->SetSkyBox(L"../Resources/Textures/SkyBox/custom1.dds");
}

void Truth::GraphicsManager::Finalize()
{

}

/// <summary>
/// 렌더
/// </summary>
void Truth::GraphicsManager::Render()
{
#ifdef EDITOR_MODE
	m_renderer->Render();
#else
	CompleteCamera();
	m_renderer->Render();
#endif // EDITOR_MODE
}


void Truth::GraphicsManager::ConvertAsset(std::wstring _path, bool _isSkind /*= false*/, bool _isData /*= false*/, bool _isCenter)
{
	m_renderer->ConvertAssetToMyFormat(_path, _isSkind, _isData, _isCenter);
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

std::shared_ptr<Ideal::IMeshObject> Truth::GraphicsManager::CreateDebugMeshObject(std::wstring _path)
{
	return m_renderer->CreateDebugMeshObject(_path);
}

std::shared_ptr<Ideal::IAnimation> Truth::GraphicsManager::CreateAnimation(std::wstring _path, const Matrix& _offset /*= Matrix::Identity*/)
{
	return m_renderer->CreateAnimation(_path, _offset);
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
/// 카메라 생성
/// </summary>
/// <returns>카메라 오브젝트</returns>
std::shared_ptr<Ideal::ICamera> Truth::GraphicsManager::CreateCamera()
{
	return m_renderer->CreateCamera();
}

/// <summary>
/// 등록된 Mesh
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
/// 메인 카메라 지정
/// </summary>
/// <param name="_camera">카메라 오브젝트</param>
void Truth::GraphicsManager::SetMainCamera(Camera* _camera)
{
	m_renderer->SetMainCamera(_camera->m_camera);
	m_mainCamera = _camera;
}

std::shared_ptr<Truth::Texture> Truth::GraphicsManager::CreateTexture(const std::wstring& _path)
{
	if (m_textureMap.find(_path) == m_textureMap.end() || m_textureMap[_path]->m_useCount <= 0)
	{
		std::shared_ptr<Texture> tex = std::make_shared<Texture>();
		tex->m_texture = m_renderer->CreateTexture(_path);
		tex->m_useCount = 1;
		tex->m_path = _path;
		return m_textureMap[_path] = tex;
	}
	return m_textureMap[_path];
}

void Truth::GraphicsManager::DeleteTexture(std::shared_ptr<Texture> _texture)
{
	_texture->m_useCount--;
	if (_texture->m_useCount <= 0)
	{
		m_renderer->DeleteTexture(_texture->m_texture);
	}
}

std::shared_ptr<Truth::Matarial> Truth::GraphicsManager::CraeteMatarial(const std::string& _name)
{
	if (m_matarialMap.find(_name) == m_matarialMap.end() || m_matarialMap[_name]->m_useCount <= 0)
	{
		std::shared_ptr<Matarial> mat = std::make_shared<Matarial>();
		mat->m_material = m_renderer->CreateMaterial();
		mat->m_name = _name;
		mat->m_baseMap = nullptr;
		mat->m_normalMap = nullptr;
		mat->m_maskMap = nullptr;
		return m_matarialMap[_name] = mat;
	}
	return m_matarialMap[_name];
}

void Truth::GraphicsManager::DeleteMaterial(std::shared_ptr<Matarial> _material)
{
	_material->m_useCount--;
	if (_material->m_useCount <= 0)
	{
		m_renderer->DeleteMaterial(_material->m_material);
	}
}

#ifdef EDITOR_MODE
void Truth::GraphicsManager::SetMainCamera(EditorCamera* _camera)
{
	m_renderer->SetMainCamera(_camera->m_camera);
	// m_mainCamera = _camera;
}
#endif // EDITOR_MODE

void Truth::GraphicsManager::CompleteCamera()
{
	m_mainCamera->CompleteCamera();
}

