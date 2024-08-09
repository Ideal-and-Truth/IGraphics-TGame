#pragma once
#include <memory>
#include <string>
#include "TypeInfo.h"

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
	struct Material
	{
		std::string m_name; 

		unsigned int m_useCount;

		std::shared_ptr<Ideal::IMaterial> m_material;

		std::shared_ptr<Texture> m_baseMap;
		std::shared_ptr<Texture> m_normalMap;
		std::shared_ptr<Texture> m_maskMap;

		void SetTexture();
	};
}


