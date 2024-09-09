#pragma once
#include <memory>
#include <string>

namespace Ideal
{
	class IShader;
	class ITexture;
}

namespace Ideal
{
	class IParticleMaterial
	{
	public:
		IParticleMaterial() {};
		virtual ~IParticleMaterial() {};

	public:
		virtual void SetShader(std::shared_ptr<Ideal::IShader> Shader) abstract;
		virtual void SetShaderByPath(const std::wstring& FilePath) abstract;
		virtual void SetTexture(std::shared_ptr<Ideal::ITexture> Texture) abstract;

	};
}