#include "Material.h"
#include "IMaterial.h"
#include "Texture.h"
#include "GraphicsManager.h"
#include "TFileUtils.h"
#include "managers.h"

void Truth::Material::SetTexture()
{
	if (m_baseMap)
		m_material->SetBaseMap(m_baseMap->m_texture);
	if (m_normalMap)
		m_material->SetNormalMap(m_normalMap->m_texture);
	if (m_maskMap)
		m_material->SetMaskMap(m_maskMap->m_texture);

	m_material->SetTiling(m_tileX, m_tileY);
}

void Truth::Material::ChangeTexture(std::wstring _path, int _type)
{
	fs::path p = fs::path(_path).extension();
	if (!(
		p == ".png" ||
		p == ".PNG" ||
		p == ".tga" ||
		p == ".TGA")
		)
	{
		return;
	}
	auto m_tex = m_gp->CreateTexture(_path);
	switch (_type)
	{
	case 0:
		m_material->SetBaseMap(m_tex->m_texture);
		m_baseMap = m_tex;
		break;
	case 1:
		m_material->SetNormalMap(m_tex->m_texture);
		m_normalMap = m_tex;
		break;
	case 2:
		m_material->SetMaskMap(m_tex->m_texture);
		m_maskMap = m_tex;
		break;
	default:
		break;
	}
}

void Truth::Material::ChangeMaterial()
{
	memset(&m_openFileName, 0, sizeof(OPENFILENAME));
	m_openFileName.lStructSize = sizeof(OPENFILENAME);
	m_openFileName.hwndOwner = m_hwnd;
	m_openFileName.lpstrFile = m_fileBuffer;
	m_openFileName.nMaxFile = 256;
	m_openFileName.lpstrInitialDir = L".";

	if (GetOpenFileName(&m_openFileName) != 0)
	{
		fs::path matPath = m_openFileName.lpstrFile;
		if (matPath.extension() != ".matData")
		{
			::SetCurrentDirectory(Managers::GetRootPath().c_str());
			return;
		}
		auto mat = m_gp->CreateMaterial(matPath.generic_string());
		m_material = mat->m_material;
		m_baseMap = mat->m_baseMap;
		m_normalMap = mat->m_normalMap;
		m_maskMap = mat->m_maskMap;
		SetTexture();
	}
	::SetCurrentDirectory(Managers::GetRootPath().c_str());
	SaveMaterial();
}

void Truth::Material::ChangeTexture(int _type)
{
	memset(&m_openFileName, 0, sizeof(OPENFILENAME));
	m_openFileName.lStructSize = sizeof(OPENFILENAME);
	m_openFileName.hwndOwner = m_hwnd;
	m_openFileName.lpstrFile = m_fileBuffer;
	m_openFileName.nMaxFile = 256;
	m_openFileName.lpstrInitialDir = L".";

	if (GetOpenFileName(&m_openFileName) != 0)
	{
		ChangeTexture(m_openFileName.lpstrFile, _type);
	}
	::SetCurrentDirectory(Managers::GetRootPath().c_str());
	SaveMaterial();
}

void Truth::Material::SaveMaterial()
{
	std::shared_ptr<TFileUtils> f = std::make_shared<TFileUtils>();
	std::filesystem::path matp = "../Resources/Matarial/" + m_name + ".matData";
	std::filesystem::path testPath = std::filesystem::current_path();
	bool b = std::filesystem::exists(matp);

	f->Open(matp, Write);
	f->Write(m_baseMap->m_path.generic_string());
	f->Write(m_normalMap->m_path.generic_string());
	f->Write(m_maskMap->m_path.generic_string());

	f->Write(m_tileX);
	f->Write(m_tileY);
}