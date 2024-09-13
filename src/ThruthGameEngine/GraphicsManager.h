#pragma once
#include "Headers.h"

namespace Truth
{
	class Mesh;
	class Camera;
	struct Material;
	struct Texture;
#ifdef EDITOR_MODE
	class EditorCamera;
#endif // EDITOR_MODE

}

namespace Ideal
{
	class IMaterial;
	class ITexture;
}

namespace Truth
{
	class GraphicsManager
	{
	private:
		std::shared_ptr<Ideal::IdealRenderer> m_renderer;
		Camera* m_mainCamera;
		float m_aspect;

		const std::string m_matSavePath = "../Resources/Matarial/";

		const wchar_t* m_assetPath[3] =
		{
			L"../Resources/Assets/",
			L"../Resources/Models/",
			L"../Resources/Textures/"
		};
	public:
		// path / texture
		std::map<std::wstring, std::shared_ptr<Texture>> m_textureMap;

		// name / matarial
		std::map<std::string, std::shared_ptr<Material>> m_matarialMap;

	public:
		GraphicsManager();
		~GraphicsManager();

		void Initalize(HWND _hwnd, uint32 _wight, uint32 _height);
		void Finalize();
		void Render();

		void ConvertAsset(std::wstring _path, bool _isSkind = false, bool _isData = false, bool _isCenter = false);

		std::shared_ptr<Ideal::ISkinnedMeshObject> CreateSkinnedMesh(std::wstring _path);
		std::shared_ptr<Ideal::IMeshObject> CreateMesh(std::wstring _path);
		std::shared_ptr<Ideal::IMeshObject> CreateDebugMeshObject(std::wstring _path);
		std::shared_ptr<Ideal::IAnimation> CreateAnimation(std::wstring _path, const Matrix& _offset = Matrix::Identity);

		std::shared_ptr<Ideal::IDirectionalLight> CreateDirectionalLight();
		std::shared_ptr<Ideal::ISpotLight> CreateSpotLight();
		std::shared_ptr<Ideal::IPointLight> CreatePointLight();

		std::shared_ptr<Ideal::ICamera> CreateCamera();

		void DeleteMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject);
		void DeleteDebugMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject);

		void SetMainCamera(Camera* _camera);

		std::shared_ptr<Texture> CreateTexture(const std::wstring& _path);
		void DeleteTexture(std::shared_ptr<Texture> _texture);

		std::shared_ptr<Material> CraeteMatarial(const std::string& _name);
		void DeleteMaterial(std::shared_ptr<Material> _material);

		std::shared_ptr<Material> GetMaterial(const std::string& _name);

		void ToggleFullScreen();

		void ResizeWindow(uint32 _w, uint32 _h);

#ifdef EDITOR_MODE
		void SetMainCamera(EditorCamera* _camera);
#endif // EDITOR_MODE


		float GetAspect() const { return m_aspect; }

		std::shared_ptr<Ideal::IdealRenderer> GetRenderer() const {return m_renderer;}

		void CompleteCamera();
	};
}

