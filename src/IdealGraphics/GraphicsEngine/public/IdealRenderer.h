#pragma once
#include <string>
#include <memory>

namespace Ideal
{
	class ICamera;
	class IMeshObject;
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

	public:
		virtual std::shared_ptr<ICamera> CreateCamera() abstract;
		virtual void SetMainCamera(std::shared_ptr<ICamera> Camera) abstract;

		virtual std::shared_ptr<Ideal::IMeshObject> CreateMeshObject(const std::wstring FileName) abstract;

	public:
		virtual void SetAssetPath(const std::wstring& AssetPath) abstract;
		virtual void SetModelPath(const std::wstring& ModelPath) abstract;
		virtual void SetTexturePath(const std::wstring& TexturePath) abstract;

		virtual void ConvertAssetToMyFormat(std::wstring FileName) abstract;
		virtual void ConvertAnimationAssetToMyFormat(std::wstring FileName) abstract;
	};
}