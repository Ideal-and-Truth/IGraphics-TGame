#pragma once
#include <string>
#include <memory>

namespace Ideal
{
	struct FontHandle;
}

namespace Ideal
{
	class IdealText
	{
	public:
		IdealText();
		~IdealText();

	public:
		void SetText(std::wstring& Text);

	private:
		std::shared_ptr<Ideal::FontHandle> m_fontHandle;
		std::wstring m_text;
		bool m_isChanged = false;
	};
}

