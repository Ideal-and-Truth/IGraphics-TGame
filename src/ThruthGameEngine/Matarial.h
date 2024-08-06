#pragma once
#include "Headers.h"

namespace Truth
{
	class Textrue;
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

		std::shared_ptr<Textrue> m_baseMap;
		std::shared_ptr<Textrue> m_normalMap;
		std::shared_ptr<Textrue> m_maskMap;
	};
}


