#pragma once
#include "Headers.h"

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

		uint32 m_useCount;
	};
}


