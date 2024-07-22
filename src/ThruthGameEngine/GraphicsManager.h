#pragma once
#include "Headers.h"

namespace Truth
{
	class Mesh;
	class Camera;
#ifdef _DEBUG
	class EditorCamera;
#endif // _DEBUG

}

namespace Truth
{
	class GraphicsManager
	{
	private:
		std::shared_ptr<Ideal::IdealRenderer> m_renderer;
		Camera* m_mainCamera;
		float m_aspect;

		const wchar_t* m_assetPath[3] =
		{
			L"../Resources/Assets/",
			L"../Resources/Models/",
			L"../Resources/Textures/"
		};

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
		std::shared_ptr<Ideal::IAnimation> CreateAnimation(std::wstring _path);

		std::shared_ptr<Ideal::IDirectionalLight> CreateDirectionalLight();
		std::shared_ptr<Ideal::ISpotLight> CreateSpotLight();
		std::shared_ptr<Ideal::IPointLight> CreatePointLight();

		std::shared_ptr<Ideal::ICamera> CreateCamera();

		void DeleteMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject);
		void DeleteDebugMeshObject(std::shared_ptr<Ideal::IMeshObject> _meshObject);

		void SetMainCamera(Camera* _camera);
#ifdef _DEBUG
		void SetMainCamera(EditorCamera* _camera);
#endif // _DEBUG


		float GetAspect() const { return m_aspect; }

		std::shared_ptr<Ideal::IdealRenderer> GetRenderer() const {return m_renderer;}

		void CompleteCamera();
	};
}

