#pragma once
#include <string>
#include <memory>

namespace Ideal
{
	class ICamera;
	class IMeshObject;
	class IAnimation;
	class ISkinnedMeshObject;
	class IRenderScene;
	class IDirectionalLight;
	class IPointLight;
	class ISpotLight;
}

namespace Ideal
{
	class IdealRenderer
	{
	public:
		IdealRenderer() {};
		virtual ~IdealRenderer() {};

	public:
		virtual void Init() abstract;
		virtual void Tick() abstract;
		virtual void Render() abstract;
		virtual void Resize(UINT Width, UINT Height) abstract;

	public:
		virtual std::shared_ptr<ICamera>					CreateCamera() abstract;
		virtual void										SetMainCamera(std::shared_ptr<ICamera> Camera) abstract;

		virtual std::shared_ptr<Ideal::IMeshObject>			CreateStaticMeshObject(const std::wstring& FileName) abstract;
		virtual std::shared_ptr<Ideal::ISkinnedMeshObject>	CreateSkinnedMeshObject(const std::wstring& FileName) abstract;
		virtual std::shared_ptr<Ideal::IAnimation>			CreateAnimation(const std::wstring& FileName) abstract;

		// CreateDebugMeshObject : 레이트레이싱에서는 그냥 Static Mesh Object 반환 //
		virtual std::shared_ptr<Ideal::IMeshObject>			CreateDebugMeshObject(const std::wstring& FileName) abstract;

		virtual void										DeleteMeshObject(std::shared_ptr<Ideal::IMeshObject> MeshObject) abstract;
		virtual void										DeleteDebugMeshObject(std::shared_ptr<Ideal::IMeshObject> DebugMeshObject) abstract;
		//virtual std::shared_ptr<Ideal::IRenderScene>		CreateRenderScene() abstract;
		//virtual void										SetRenderScene(std::shared_ptr<Ideal::IRenderScene> RenderScene) abstract;

		virtual std::shared_ptr<Ideal::IDirectionalLight>	CreateDirectionalLight() abstract;
		virtual std::shared_ptr<Ideal::ISpotLight>			CreateSpotLight() abstract;
		virtual std::shared_ptr<Ideal::IPointLight>			CreatePointLight() abstract;

		virtual void SetSkyBox(const std::wstring& FileName) abstract;

	public:
		virtual void SetAssetPath(const std::wstring& AssetPath) abstract;
		virtual void SetModelPath(const std::wstring& ModelPath) abstract;
		virtual void SetTexturePath(const std::wstring& TexturePath) abstract;

		virtual void ConvertAssetToMyFormat(std::wstring FileName, bool isSkinnedData = false, bool NeedVertexInfo = false, bool NeedConvertCenter = false) abstract;
		virtual void ConvertAnimationAssetToMyFormat(std::wstring FileName) abstract;

	public:
		virtual bool SetImGuiWin32WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) abstract;
		virtual void ClearImGui() abstract;
	};
}