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
		// �ؽ��ĸ� ������ �ִ´�...
		std::weak_ptr<Ideal::D3D12Texture> m_texture;
		// ��������Ʈ ����!
		CB_Sprite m_cbSprite;
	};
}