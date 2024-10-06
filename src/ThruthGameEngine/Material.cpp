#include "Material.h"
#include "IMaterial.h"
#include "Texture.h"
#include "GraphicsManager.h"
#include "FileUtils.h"

void Truth::Material::SetTexture()
{
	if (m_baseMap)
		m_material->SetBaseMap(m_baseMap->m_texture);
	if (m_normalMap)
		m_material->SetNormalMap(m_normalMap->m_texture);
	if (m_maskMap)
		m_material->SetMaskMap(m_maskMap->m_texture);

	// m_material->SetTiling(m_tileX, m_tileY);
}

void Truth::Material::ChangeTexture(std::wstring _path, int _type)
{
	auto m_tex = m_gp->CreateTexture(_path);
	switch (_type)
	{
	case 0:
		m_material->SetBaseMap(m_tex->m_texture);
		break;
	case 1:
		m_material->SetNormalMap(m_tex->m_texture);
		break;
	case 2:
		m_material->SetMaskMap(m_tex->m_texture);
		break;
	default:
		break;
	}
}
void Truth::Material::SaveMaterial()
{
	std::shared_ptr<FileUtils> f = std::make_shared<FileUtils>();
	std::filesystem::path matp = "../Resources/Matarial/" + m_name + ".matData";
	f->Open(matp, Write);
	f->Write(m_baseMap->m_path);
	f->Write(m_normalMap->m_path);
	f->Write(m_maskMap->m_path);

}