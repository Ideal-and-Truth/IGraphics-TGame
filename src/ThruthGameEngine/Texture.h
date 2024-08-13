#pragma once
#include <memory>
#include <string>
#include "ITexture.h"

namespace Ideal
{
	class ITexture;
}

namespace Truth
{
	struct Texture
	{
	public:
		std::wstring m_path; 
		std::shared_ptr<Ideal::ITexture> m_texture;

		unsigned long long GetImageID()
		{
			return m_texture->GetImageID();
		}

		unsigned int m_useCount;
	};
}


