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
	namespace ParticleMaterialMenu
	{
		enum class EBlendingMode
		{
			Additive = 0,
			Alpha,
		};
	}
}

namespace Ideal
{
	class IParticleMaterial
	{
	public:
		IParticleMaterial() {};
		virtual ~IParticleMaterial() {};

	public:
		virtual void SetBlendingMode(Ideal::ParticleMaterialMenu::EBlendingMode BlendingMode) abstract;
		virtual void SetShader(std::shared_ptr<Ideal::IShader> Shader) abstract;
		virtual void SetShaderByPath(const std::wstring& FilePath) abstract;
		virtual void SetTexture0(std::shared_ptr<Ideal::ITexture> Texture) abstract;
		virtual void SetTexture1(std::shared_ptr<Ideal::ITexture> Texture) abstract;
		virtual void SetTexture2(std::shared_ptr<Ideal::ITexture> Texture) abstract;

	};
}