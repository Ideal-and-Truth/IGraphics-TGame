#pragma once
#include "../Utils/SimpleMath.h"
#include <memory>

namespace Ideal
{
	class ITexture;
}

namespace Ideal
{
	class ISprite
	{
	public:
		ISprite() {}
		~ISprite() {}

	public:
		virtual DirectX::SimpleMath::Vector2 const& GetPosition() abstract;
		virtual DirectX::SimpleMath::Vector2 const& GetSamplePosition() abstract;
		virtual DirectX::SimpleMath::Vector2 const& GetSampleSize() abstract;
		virtual float GetZ() abstract;
		virtual float GetAlpha() abstract;
		virtual void SetScreenPosition(const DirectX::SimpleMath::Vector2& ScreenPos) abstract;
		virtual void SetPosition(const DirectX::SimpleMath::Vector2&) abstract;
		virtual void SetTextureSize(const DirectX::SimpleMath::Vector2&) abstract;
		virtual void SetTextureSamplePosition(const DirectX::SimpleMath::Vector2&) abstract;
		virtual void SetTextureSampleSize(const DirectX::SimpleMath::Vector2&) abstract;
		virtual void SetZ(float) abstract;
		virtual void SetAlpha(float) abstract;
		virtual void SetTexture(std::weak_ptr<Ideal::ITexture>) abstract;
	};
}