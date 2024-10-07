#pragma once
#include <memory>
#include <string>
#include "TypeInfo.h"

namespace Truth
{
	struct Texture;
	class GraphicsManager;
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

		std::shared_ptr<GraphicsManager> m_gp;

		std::shared_ptr<Texture> m_baseMap;
		std::shared_ptr<Texture> m_normalMap;
		std::shared_ptr<Texture> m_maskMap;

		float m_tileX = 1.0f;
		float m_tileY = 1.0f;

		void SetTexture();
		void ChangeTexture(std::wstring _path, int _type);
		void SaveMaterial();
	};
}


