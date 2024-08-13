#include "Scene.h"
#include "Entity.h"
#include "Managers.h"
#include "EventManager.h"
#include "GraphicsManager.h"
#include "NavMeshGenerater.h"
#include "PhysicsManager.h"
#include "FileUtils.h"
#include "ISpotLight.h"
#include "Material.h"
#include "IMesh.h"

/// <summary>
/// 생성자
/// </summary>
/// <param name="_managers">매니저</param>
Truth::Scene::Scene(std::shared_ptr<Managers> _managers)
	: m_managers(_managers)
	, m_name("No Name Scene")
	, m_mapPath(L"SampleScene")
{
}

/// <summary>
/// 소멸자
/// </summary>
Truth::Scene::~Scene()
{
	ClearEntity();
	for (auto& m : m_mapMesh)
	{
		m_managers.lock()->Graphics()->DeleteMeshObject(m);
	}
}

/// <summary>
/// 엔티티 추가 (Scene 시작 전에 들어가는)
/// </summary>
/// <param name="_p">엔티티</param>
void Truth::Scene::AddEntity(std::shared_ptr<Entity> _p)
{
	_p->m_index = static_cast<int32>(m_entities.size());
	m_entities.push_back(_p);
	_p->Initialize();
	m_awakedEntity.push(_p);

	if (_p->m_parent.expired())
	{
		m_rootEntities.push_back(_p);
	}
}

/// <summary>
/// 엔티티 생성 (Scene 시작 후 중간에 생성되는 엔티티)
/// </summary>
/// <param name="_p"></param>
void Truth::Scene::CreateEntity(std::shared_ptr<Entity> _p)
{
	m_awakedEntity.push(_p);
}

/// <summary>
/// 엔티티 삭제
/// </summary>
/// <param name="_index">삭제될 엔티티 인덱스</param>
void Truth::Scene::DeleteEntity(uint32 _index)
{
	if (_index >= m_entities.size())
	{
		return;
	}
	m_beginDestroy.push(m_entities[_index]);
}

/// <summary>
/// 엔티티 삭제
/// </summary>
/// <param name="_p">삭제될 엔티티</param>
void Truth::Scene::DeleteEntity(std::shared_ptr<Entity> _p)
{
	for (auto& child : _p->m_children)
	{
		DeleteEntity(child);
	}
#ifdef EDITOR_MODE
	if (m_managers.lock()->m_isEdit)
	{
		_p->Destroy();
		m_entities.back()->m_index = _p->m_index;
		std::iter_swap(m_entities.begin() + _p->m_index, m_entities.begin() + (m_entities.size() - 1));
		m_entities.pop_back();
	}
	else
	{
		m_beginDestroy.push(_p);
	}
#else
	m_beginDestroy.push(_p);
#endif // EDITOR_MODE
}

/// <summary>
/// 씬 초기화 (Load Scene 하는 경우)
/// </summary>
/// <param name="_manager"></param>
void Truth::Scene::Initalize(std::weak_ptr<Managers> _manager)
{
	m_managers = _manager;
	for (auto& e : m_rootEntities)
	{
		LoadEntity(e);
	}


	//CreateMap(m_mapPath);
	CreateMap(L"SampleScene");
}

void Truth::Scene::LoadEntity(std::shared_ptr<Entity> _entity)
{
	m_entities.push_back(_entity);
	_entity->SetManager(m_managers);
	m_awakedEntity.push(_entity);
	_entity->Initialize();

	for (auto& child : _entity->m_children)
	{
		child->m_parent = _entity;
		LoadEntity(child);
	}
}

DirectX::SimpleMath::Vector3 Truth::Scene::FindPath(Vector3 _start, Vector3 _end, Vector3 _size) const
{
	return m_navMesh->FindPath(_start, _end, _size);
}

std::weak_ptr<Truth::Entity> Truth::Scene::FindEntity(std::string _name)
{
	for (auto& e : m_entities)
	{
		if (e->m_name == _name)
		{
			return e;
		}
	}
	return std::weak_ptr<Entity>();
}

void Truth::Scene::ResetMapData()
{
	m_managers.lock()->Physics()->ResetPhysX();
	for (auto& m : m_mapMesh)
	{
		m_managers.lock()->Graphics()->DeleteMeshObject(m);
	}
}

#ifdef EDITOR_MODE
void Truth::Scene::EditorUpdate()
{
	m_rootEntities.clear();
	for (auto& e : m_entities)
	{
		if (e->m_parent.expired())
		{
			m_rootEntities.push_back(e);
		}
	}
}
#endif // EDITOR_MODE

/// <summary>
/// 씬 업데이트
/// </summary>
void Truth::Scene::Update()
{
	// delete
	while (!m_finishDestroy.empty())
	{
		m_finishDestroy.pop();
	}
	while (!m_beginDestroy.empty())
	{
		auto& e = m_beginDestroy.front();
		if (e->m_isDead)
		{
			m_beginDestroy.pop();
			continue;
		}
		e->Destroy();
		m_entities.back()->m_index = e->m_index;
		std::iter_swap(m_entities.begin() + e->m_index, m_entities.begin() + (m_entities.size() - 1));
		m_finishDestroy.push(e);
		m_entities.pop_back();
		m_beginDestroy.pop();
	}

	// Create
	while (!m_awakedEntity.empty())
	{
		auto& e = m_awakedEntity.front();
		e->m_index = static_cast<int32>(m_entities.size());
		m_entities.push_back(e);
#ifdef EDITOR_MODE
		m_rootEntities.push_back(e);
#endif // EDITOR_MODE
		m_startedEntity.push(e);
		e->Awake();  
		m_awakedEntity.pop();
	}
	while (!m_startedEntity.empty())
	{
		auto& e = m_startedEntity.front();
		e->Start();
		m_startedEntity.pop();
	}
	for (auto& e : m_entities)
	{
		if (!e->m_isDead)
		{
			e->Update();
		}
		else
		{
			m_beginDestroy.push(e);
		}
	}
}

void Truth::Scene::FixedUpdate()
{
	for (auto& e : m_entities)
	{
		if (!e->m_isDead)
		{
			e->FixedUpdate();
		}
		else
		{
			m_beginDestroy.push(e);
		}
	}
}

void Truth::Scene::LateUpdate()
{
	for (auto& e : m_entities)
	{
		if (!e->m_isDead)
		{
			e->LateUpdate();
		}
		else
		{
			m_beginDestroy.push(e);
		}
	}
}

/// <summary>
/// 모든 씬의 Transform 정보 갱신
/// </summary>
void Truth::Scene::ApplyTransform()
{
	for (auto& e : m_entities)
	{
		e->ApplyTransform();
	}
}

/// <summary>
/// 로드 된 씬을 시작 할 때 단 한번 실행되는 함수
/// </summary>
void Truth::Scene::Start()
{
	while (!m_awakedEntity.empty())
	{
		auto& e = m_awakedEntity.front();
		m_startedEntity.push(e);
		e->Awake();
		m_awakedEntity.pop();
	}
	while (!m_startedEntity.empty())
	{
		auto& e = m_startedEntity.front();
		e->Start();
		m_startedEntity.pop();
	}
}

/// <summary>
/// 씬 진입 시
/// </summary>
void Truth::Scene::Enter()
{
	int32 index = 0;
	for (auto& e : m_entities)
	{
		e->m_index = index++;
	}
}

/// <summary>
/// 씬 나갈 시
/// </summary>
void Truth::Scene::Exit()
{
	ClearEntity();
}

/// <summary>
/// 모든 엔티티 초기화
/// </summary>
void Truth::Scene::ClearEntity()
{
	m_entities.clear();
}

void Truth::Scene::CreateMap(const std::wstring& _path)
{
	if (_path.empty())
	{
		return;
	}

	auto gp = m_managers.lock()->Graphics();

	std::wstring mapPath = L"../Resources/MapData/" + _path + L"/";

	std::filesystem::path p(L"../Resources/MapData/" + _path);
	if (!std::filesystem::exists(p))
	{
		return;
	}
	m_managers.lock()->Physics()->CreateMapCollider(mapPath + L"Data.map");

	m_navMesh = std::make_shared<NavMeshGenerater>();
	m_navMesh->Initalize(mapPath + L"Data.map");

	std::shared_ptr<FileUtils> matFile = std::make_shared<FileUtils>();
	std::wstring matPath = mapPath + L"Material.mats";
	matFile->Open(matPath, FileMode::Read);

	size_t matCount = matFile->Read<size_t>();
	for (size_t i = 0; i < matCount; i++)
	{
		std::string guid = matFile->Read<std::string>();
		std::string name = matFile->Read<std::string>();
		std::string baseMap = matFile->Read<std::string>();
		std::string normalMap = matFile->Read<std::string>();
		std::string maskMap = matFile->Read<std::string>();


		USES_CONVERSION;
		std::shared_ptr<Texture> baseTex = gp->CreateTexture(A2W(baseMap.c_str()));
		std::shared_ptr<Texture> normalTex = gp->CreateTexture(A2W(normalMap.c_str()));
		std::shared_ptr<Texture> maskTex = gp->CreateTexture(A2W(maskMap.c_str()));

		std::shared_ptr<Material> mat = gp->CraeteMatarial(name);
		mat->m_baseMap = baseTex;
		mat->m_normalMap = normalTex;
		mat->m_maskMap = maskTex;
		mat->SetTexture();
	}

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	std::wstring path = mapPath + L"Meshes.mList";
	file->Open(path, FileMode::Read);

	uint32 meshCount = file->Read<uint32>();
	m_mapMesh.resize(meshCount);

	for (uint32 i = 0; i < meshCount; i++)
	{
		std::string meshpath = file->Read<std::string>();
		Matrix meshTM = file->Read<Matrix>();

		size_t matCount = file->Read<size_t>();
		std::vector<std::string> matName;
		for (uint32 i = 0; i < matCount; i++)
		{
			matName.push_back(file->Read<std::string>());
		}

		USES_CONVERSION;
		std::wstring wsval(A2W(meshpath.c_str()));

		m_mapMesh.push_back(m_managers.lock()->Graphics()->CreateMesh(wsval));

		Matrix flipYZ = Matrix::Identity;
		flipYZ.m[0][0] = -1.f;

		Matrix flipXY = Matrix::Identity;
		flipXY.m[2][2] = -1.f;

		m_mapMesh.back()->SetTransformMatrix(flipYZ * flipXY * meshTM);

		if (matCount > 0)
		{
			auto& mesh = m_mapMesh.back();
			uint32 meshSize = mesh->GetMeshesSize();
			for (uint32 i = 0; i < mesh->GetMeshesSize(); i++)
			{
				auto submesh = mesh->GetMeshByIndex(i).lock();
				std::string smat = submesh->GetFBXMaterialName();
				submesh->SetMaterialObject(gp->m_matarialMap[smat]->m_material);
			}
		}
	}

	std::shared_ptr<FileUtils> lightFile = std::make_shared<FileUtils>();
	std::wstring lightpath = mapPath + L"Lights.lList";
	lightFile->Open(lightpath, FileMode::Read);

	uint32 lightCount = lightFile->Read<uint32>();
	for (uint32 i = 0; i < lightCount; i++)
	{
		uint32 type = lightFile->Read<uint32>();

		float intensity = lightFile->Read<float>();
		Color color =
		{
			lightFile->Read<float>(),
			lightFile->Read<float>(),
			lightFile->Read<float>(),
			lightFile->Read<float>()
		};

		float range = lightFile->Read<float>();
		float angle = lightFile->Read<float>();

		Matrix worldTM = lightFile->Read<Matrix>();

		Vector3 dir = { 0.0f, 0.0f, 1.0f };

		Vector3 pos;
		Vector3 sca;
		Quaternion rot;
		worldTM.Decompose(sca, rot, pos);

		Matrix rotMat = Matrix::CreateFromQuaternion(rot);

		dir = Vector3::Transform(dir, rotMat);

		switch (type)
		{
			// spot
		case 0:
		{
			std::shared_ptr<Ideal::ISpotLight> spotlight
				= m_managers.lock()->Graphics()->CreateSpotLight();
			spotlight->SetIntensity(intensity);
			spotlight->SetLightColor(color);
			spotlight->SetDirection(dir);
			spotlight->SetRange(range);
			spotlight->SetPosition(pos);
			spotlight->SetSpotAngle(angle);
			m_mapLight.push_back(spotlight);
			break;
		}
		// direction
		case 1:
		{
			// 			std::shared_ptr<Ideal::IDirectionalLight> directionlight
			// 				 = m_managers.lock()->Graphics()->CreateDirectionalLight();
			// 			directionlight->SetIntensity(intensity);
			// 			directionlight->SetDiffuseColor(color);
			// 			directionlight->SetDirection(dir);
			// 			m_mapLight.push_back(directionlight);
			//  			break;
		}
		// point
		case 2:
		{
			std::shared_ptr<Ideal::IPointLight> pointLight
				= m_managers.lock()->Graphics()->CreatePointLight();
			pointLight->SetIntensity(intensity);
			pointLight->SetLightColor(color);
			pointLight->SetRange(range);
			pointLight->SetPosition(pos);
			m_mapLight.push_back(pointLight);
			break;
		}
		default:
			break;
		}
	}
}

