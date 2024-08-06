#pragma once
#include "ISprite.h"
#include "GraphicsEngine/ConstantBufferInfo.h"
#include <memory>

namespace Ideal
{
	class D3D12Texture;
}

namespace Ideal
{
	class IdealSprite : public ISprite
	{
	public:
		IdealSprite();
		virtual ~IdealSprite();

	public:
		void SetScreenPosition(Vector2 ScreenPos);
		void SetPosition(Vector2 Position);
		void SetTextureSize(Vector2 TextureSize);
		void SetTextureSamplePosition(Vector2 TextureSamplePosition);
		void SetTextureSampleSize(Vector2 TextureSampleSize);
		void SetZ(float Z);
		void SetAlpha(float Alpha);

	private:
		// 텍스쳐를 가지고 있는다...
		std::weak_ptr<Ideal::D3D12Texture> m_texture;
		// 스프라이트 정보!
		CB_Sprite m_cbSprite;
	};
}