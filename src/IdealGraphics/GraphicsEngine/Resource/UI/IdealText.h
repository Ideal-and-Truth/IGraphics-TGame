#pragma once
#include "IText.h"
#include "Core/Core.h"
#include <string>
#include <memory>

namespace Ideal
{
	struct FontHandle;
	class IdealSprite;
	class D2DTextManager;
	class D3D12Texture;
}
struct ID3D12Device;

namespace Ideal
{
	class IdealText : public IText
	{
	public:
		IdealText(std::shared_ptr<Ideal::D2DTextManager> TextManager, std::shared_ptr<Ideal::FontHandle> FontHandle);
		~IdealText();

	public:
		virtual void ChangeText(const std::wstring& Text) override;
		std::shared_ptr<Ideal::IdealSprite> GetSprite();
		void SetSprite(std::shared_ptr<Ideal::IdealSprite> Sprite);
		// 자기만의 텍스쳐를 가진다.
		void SetTexture(std::shared_ptr<Ideal::D3D12Texture> Texture);
		void SetFontHandle(std::shared_ptr<Ideal::FontHandle> FontHandle);
		void UpdateDynamicTextureWithImage(ComPtr<ID3D12Device> Device);

	private:
		std::weak_ptr<Ideal::D2DTextManager> m_textManager;
		std::shared_ptr<Ideal::D3D12Texture> m_texture;
		std::shared_ptr<Ideal::FontHandle> m_fontHandle;
		std::shared_ptr<Ideal::IdealSprite> m_textSprite;
		std::wstring m_text;
		bool m_isChanged = false;
		BYTE* textImage = nullptr;
	};
}

