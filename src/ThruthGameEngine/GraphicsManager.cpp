#include "GraphicsManager.h"
#include "IRenderScene.h"
#include "ISkinnedMeshObject.h"
#include "imgui.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"
#include <filesystem>

#ifdef EDITOR_MODE
#include "EditorCamera.h"
#endif // EDITOR_MODE
#include "FileUtils.h"


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
	for (auto& mat : m_matarialMap)
	{
		m_renderer->DeleteMaterial(mat.second->m_material);
	}

	for (auto& tex : m_textureMap)
	{
		m_renderer->DeleteTexture(tex.second->m_texture);
	}
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
	m_hwnd = _hwnd;
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

	m_renderer->SetDisplayResolutionOption(Ideal::Resolution::EDisplayResolutionOption::R_1920_1080);
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
	if (m_mainCamera)
	{
		m_mainCamera->CompleteCamera();
	}
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

std::shared_ptr<Ideal::ISprite> Truth::GraphicsManager::CreateSprite()
{
	return m_renderer->CreateSprite();
}

void Truth::GraphicsManager::DeleteSprite(std::shared_ptr<Ideal::ISprite> _sprite)
{
	m_renderer->DeleteSprite(_sprite);
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
	if (m_textureMap.find(_path) == m_textureMap.end())
	{
		std::shared_ptr<Texture> tex = std::make_shared<Texture>();
		std::filesystem::path p(_path);
		if (_path.empty())
		{
			return nullptr;
		}
// 		else if (p.extension() == ".tga" || p.extension() == ".TGA")
// 		{
// 			return nullptr;
// 		}
		tex->m_texture = m_renderer->CreateTexture(_path);
		tex->m_useCount = 1;
		tex->m_path = _path;

		tex->w = tex->m_texture->GetWidth();
		tex->h = tex->m_texture->GetHeight();
		return m_textureMap[_path] = tex;
	}
	return m_textureMap[_path];
}

void Truth::GraphicsManager::DeleteTexture(std::shared_ptr<Texture> _texture)
{
	m_renderer->DeleteTexture(_texture->m_texture);
}

std::shared_ptr<Truth::Material> Truth::GraphicsManager::CraeteMatarial(const std::string& _name)
{
	if (m_matarialMap.find(_name) == m_matarialMap.end())
	{
		std::filesystem::path matp = m_matSavePath + _name + ".matData";
		std::shared_ptr<Material> mat = std::make_shared<Material>();
		mat->m_material = m_renderer->CreateMaterial();
		mat->m_name = _name;
		mat->m_baseMap = nullptr;
		mat->m_normalMap = nullptr;
		mat->m_maskMap = nullptr;
		
		if (std::filesystem::exists(matp))
		{
			std::shared_ptr<FileUtils> f = std::make_shared<FileUtils>();
			f->Open(matp, Read);
			std::filesystem::path albedo(f->Read<std::string>());
			std::filesystem::path normal(f->Read<std::string>());
			std::filesystem::path metalicRoughness(f->Read<std::string>());
			mat->m_baseMap = CreateTexture(albedo);
			mat->m_normalMap = CreateTexture(normal);
			mat->m_maskMap = CreateTexture(metalicRoughness);
			mat->SetTexture();
		}
		else
		{
			OPENFILENAME m_openFileName;
			TCHAR m_filePathBuffer[256] = L"";
			TCHAR m_fileBuffer[256] = L"";
			
			memset(&m_openFileName, 0, sizeof(OPENFILENAME));
			m_openFileName.lStructSize = sizeof(OPENFILENAME);
			m_openFileName.hwndOwner = m_hwnd;
			m_openFileName.lpstrFile = m_fileBuffer;
			m_openFileName.nMaxFile = 256;
			m_openFileName.lpstrInitialDir = L".";


			std::shared_ptr<FileUtils> f = std::make_shared<FileUtils>();
			f->Open(matp, Write);

			fs::path al = "../Resources/DefaultData/DefaultAlbedo.png";
			fs::path no = "../Resources/DefaultData/DefaultNormalMap.png";
			fs::path ma = "../Resources/DefaultData/DefaultBlack.png";

			if (GetOpenFileName(&m_openFileName) != 0)
			{
				al = m_openFileName.lpstrFile;
			}
			if (GetOpenFileName(&m_openFileName) != 0)
			{
				no = m_openFileName.lpstrFile;
			}
			if (GetOpenFileName(&m_openFileName) != 0)
			{
				ma = m_openFileName.lpstrFile;
			}
			std::string rootPath = "../Resources/Textures/PlayerRe/adsf/";

			f->Write(rootPath + al.filename().generic_string());
			f->Write(rootPath + no.filename().generic_string());
			f->Write(rootPath + ma.filename().generic_string());
			std::filesystem::path albedo(rootPath + al.filename().generic_string());
			std::filesystem::path normal(rootPath + no.filename().generic_string());
			std::filesystem::path metalicRoughness(rootPath + ma.filename().generic_string());
			
			mat->m_baseMap = CreateTexture(al.generic_wstring());
			mat->m_normalMap = CreateTexture(no.generic_wstring());
			mat->m_maskMap = CreateTexture(ma.generic_wstring());
			mat->SetTexture();
		}

		return m_matarialMap[_name] = mat;
	}
	return m_matarialMap[_name];
}

void Truth::GraphicsManager::DeleteMaterial(std::shared_ptr<Material> _material)
{
	m_renderer->DeleteMaterial(_material->m_material);
}

std::shared_ptr<Ideal::IParticleMaterial> Truth::GraphicsManager::CreateParticleMaterial()
{
	return m_renderer->CreateParticleMaterial();
}

void Truth::GraphicsManager::DeleteParticleMaterial(std::shared_ptr<Ideal::IParticleMaterial> _material)
{
	m_renderer->DeleteParticleMaterial(_material);
}

std::shared_ptr<Ideal::IMesh> Truth::GraphicsManager::CreateParticleMesh(const std::wstring& _name)
{
	if (m_particleMeshMap.find(_name) != m_particleMeshMap.end())
	{
		return m_particleMeshMap[_name];
	}
	m_particleMeshMap[_name] = m_renderer->CreateParticleMesh(_name);
	return m_particleMeshMap[_name];
}

void Truth::GraphicsManager::DeleteParticleMesh(std::shared_ptr<Ideal::IMesh> _mesh)
{
	return;
}

std::shared_ptr<Ideal::IParticleSystem> Truth::GraphicsManager::CreateParticle(std::shared_ptr<Ideal::IParticleMaterial> _mat)
{
	return m_renderer->CreateParticleSystem(_mat);
}

void Truth::GraphicsManager::DeleteParticle(std::shared_ptr<Ideal::IParticleSystem> _particle)
{
	m_renderer->DeleteParticleSystem(_particle);
}

std::shared_ptr<Ideal::IShader> Truth::GraphicsManager::CreateShader(const std::wstring& _name)
{
	return m_renderer->CreateAndLoadParticleShader(_name);
}

std::shared_ptr<Truth::Material> Truth::GraphicsManager::GetMaterial(const std::string& _name)
{
	return m_matarialMap[_name];
}

void Truth::GraphicsManager::ToggleFullScreen()
{
	m_renderer->ToggleFullScreenWindow();
}

void Truth::GraphicsManager::ResizeWindow(uint32 _w, uint32 _h)
{
	m_renderer->Resize(_w, _h);
}

void Truth::GraphicsManager::BakeStaticMesh()
{
	m_renderer->BakeOption(32, 10.f);
	m_renderer->BakeStaticMeshObject();
	m_renderer->ReBuildBLASFlagOn();
}

#ifdef EDITOR_MODE
void Truth::GraphicsManager::SetMainCamera(EditorCamera* _camera)
{
	m_renderer->SetMainCamera(_camera->m_camera);
}
#endif // EDITOR_MODE

void Truth::GraphicsManager::CompleteCamera()
{
	m_mainCamera->CompleteCamera();
}

