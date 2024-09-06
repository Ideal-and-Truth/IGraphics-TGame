#include "Mesh.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "ISkinnedMeshObject.h"
#include "Entity.h"
#include "IMesh.h"
#include "Material.h"
#include "ITexture.h"
#include "Texture.h"
#include "IMaterial.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Mesh)

/// <summary>
/// 생성자
/// </summary>
/// <param name="_path">해당 매쉬가 있는 경로</param>
Truth::Mesh::Mesh(std::wstring _path)
	: Component()
	, m_path(_path)
	, m_isRendering(true)
	, m_mesh(nullptr)
{
	m_name = "Mesh Filter";
}

/// <summary>
/// 기본 생성자
/// </summary>
Truth::Mesh::Mesh()
	: Component()
	, m_path(L"DebugObject/debugCube")
	, m_isRendering(true)
	, m_mesh(nullptr)
{
	m_name = "Mesh Filter";
}

/// <summary>
/// 소멸자
/// </summary>
Truth::Mesh::~Mesh()
{
	DeleteMesh();
}

/// <summary>
/// 새로운 매쉬 지정
/// </summary>
/// <param name="_path">경로</param>
void Truth::Mesh::SetMesh(std::wstring _path)
{
	m_path = _path;

// 	if (m_mesh != nullptr)
// 	{
// 		DeleteMesh();
// 	}

	m_mesh = m_managers.lock()->Graphics()->CreateMesh(_path);

	uint32 meshSize = m_mesh->GetMeshesSize();
	m_subMesh.clear();
	m_subMesh.resize(meshSize);
	m_mat.clear();
	for (uint32 i = 0; i < meshSize; i++)
	{
		m_subMesh[i] = m_mesh->GetMeshByIndex(i).lock();
		m_mat.push_back(m_subMesh[i]->GetMaterialObject().lock());
	}
}

void Truth::Mesh::SetMesh()
{
	SetMesh(m_path);
}

void Truth::Mesh::SetRenderable(bool _isRenderable)
{
}

void Truth::Mesh::Initalize()
{
	SetMesh(m_path);
}

void Truth::Mesh::ApplyTransform()
{
	m_mesh->SetTransformMatrix(m_owner.lock()->GetWorldTM());
	m_mesh->SetDrawObject(m_isRendering);
}

void Truth::Mesh::DeleteMesh()
{
	m_managers.lock()->Graphics()->DeleteMeshObject(m_mesh);
}

void Truth::Mesh::SetMaterialByIndex(uint32 _index, std::string _material)
{
	if (_index >= m_subMesh.size())
	{
		return;
	}
	m_subMesh[_index]->SetMaterialObject(m_managers.lock()->Graphics()->CraeteMatarial(_material)->m_material);
}

std::vector<uint64> Truth::Mesh::GetMaterialImagesIDByIndex(uint32 _index)
{
	if (_index >= m_subMesh.size())
	{
		return std::vector<uint64>();
	}
	std::vector<uint64> result;
	std::string matName = m_subMesh[_index]->GetFBXMaterialName();
	std::shared_ptr<Material> mat = m_managers.lock()->Graphics()->GetMaterial(matName);

	result.push_back(mat->m_baseMap->m_texture->GetImageID());
	result.push_back(mat->m_normalMap->m_texture->GetImageID());
	result.push_back(mat->m_maskMap->m_texture->GetImageID());

	return result;
}

#ifdef EDITOR_MODE
void Truth::Mesh::EditorSetValue()
{
	SetMesh(m_path);
}
#endif // EDITOR_MODE

