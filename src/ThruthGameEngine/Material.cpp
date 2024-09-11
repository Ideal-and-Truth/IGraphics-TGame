#include "Material.h"
#include "IMaterial.h"
#include "Texture.h"

void Truth::Material::SetTexture()
{
	if (m_baseMap)
		m_material->SetBaseMap(m_baseMap->m_texture);
	if (m_normalMap)
		m_material->SetNormalMap(m_normalMap->m_texture);
	if (m_maskMap)
		m_material->SetMaskMap(m_maskMap->m_texture);
}
