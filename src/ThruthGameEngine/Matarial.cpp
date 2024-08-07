#include "Matarial.h"
#include "IMaterial.h"
#include "Texture.h"

void Truth::Matarial::SetTexture()
{
	m_material->SetBaseMap(m_baseMap->m_texture);
	m_material->SetNormalMap(m_normalMap->m_texture);
	m_material->SetMaskMap(m_maskMap->m_texture);
}
