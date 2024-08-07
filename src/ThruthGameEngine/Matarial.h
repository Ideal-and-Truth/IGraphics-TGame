#pragma once
#include "Headers.h"

namespace Truth
{
	struct Texture;
}
namespace Ideal
{
	class IMaterial;
}

namespace Truth
{
	struct Matarial
	{
		std::string m_name; 

		uint32 m_useCount;

		std::shared_ptr<Ideal::IMaterial> m_material;

		std::shared_ptr<Texture> m_baseMap;
		std::shared_ptr<Texture> m_normalMap;
		std::shared_ptr<Texture> m_maskMap;

		void SetTexture();
	};
}


