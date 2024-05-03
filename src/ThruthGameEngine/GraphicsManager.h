#pragma once
#include "Headers.h"

class EMesh;
namespace Truth
{
	class GraphicsManager
	{
	private:
		std::shared_ptr<Ideal::IdealRenderer> m_renderer;
		std::shared_ptr<Ideal::IRenderScene> m_renderScene;

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

		void AddObject(std::shared_ptr<EMesh>);
		void AddAnimation(std::shared_ptr<EMesh>);

		void ConvertAsset(std::wstring _path, bool _isSkind);

		void CreateMesh(std::wstring _path);
		void CreateAnimation(std::wstring _path);
	};
}

