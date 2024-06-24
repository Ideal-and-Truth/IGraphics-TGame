#pragma once
#include "Headers.h"

namespace Truth
{
	class Mesh;
	class Camera;
}

namespace Truth
{
	class GraphicsManager
	{
	private:
		std::shared_ptr<Ideal::IdealRenderer> m_renderer;
		std::shared_ptr<Ideal::IRenderScene> m_renderScene;
		std::shared_ptr<Camera> m_mainCamera;
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

		void AddObject(std::shared_ptr<Ideal::IMeshObject> _mesh);
		void AddLight(std::shared_ptr<Ideal::ILight> _light);
		void AddDebugobject(std::shared_ptr<Ideal::IMeshObject> _mesh);

		void ConvertAsset(std::wstring _path, bool _isSkind);

		std::shared_ptr<Ideal::ISkinnedMeshObject> CreateSkinnedMesh(std::wstring _path);
		std::shared_ptr<Ideal::IMeshObject> CreateMesh(std::wstring _path);
		std::shared_ptr<Ideal::IAnimation> CreateAnimation(std::wstring _path);

		std::shared_ptr<Ideal::IDirectionalLight> CreateDirectionalLight();
		std::shared_ptr<Ideal::ISpotLight> CreateSpotLight();
		std::shared_ptr<Ideal::IPointLight> CreatePointLight();

		std::shared_ptr<Ideal::ICamera> CreateCamera();
		void SetMainCamera(std::shared_ptr<Ideal::ICamera> _camera);

		float GetAspect() const { return m_aspect; }

		std::shared_ptr<Ideal::IdealRenderer> GetRenderer() const {return m_renderer;}

		void ResetRenderScene();
	};
}

