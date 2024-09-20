#pragma once
#include <memory>
#include <string>
#include "ITexture.h"
#include "Types.h"

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

		uint32 w;
		uint32 h;

		unsigned long long GetImageID() const
		{
			return m_texture->GetImageID();
		}

		unsigned int m_useCount;
	};
}


