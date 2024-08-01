#pragma once
#include <memory>

namespace Ideal
{
	class ITexture;
}

namespace Ideal
{
	class IMaterial
	{
	public:
		IMaterial() {}
		virtual ~IMaterial() {}

	public:
		virtual void SetBaseMap(std::shared_ptr<Ideal::ITexture> Texture) abstract;
		virtual void SetNormalMap(std::shared_ptr<Ideal::ITexture> Texture) abstract;
		// R : Metallic, A : Smoothness
		virtual void SetMaskMap(std::shared_ptr<Ideal::ITexture> Texture) abstract;
	};
}