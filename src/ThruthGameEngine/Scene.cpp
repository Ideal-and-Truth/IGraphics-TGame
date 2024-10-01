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
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "Mesh.h"

#include <algorithm>

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
	if (!_p->m_parent.expired())
	{
		_p->m_parent.lock()->DeleteChild(_p);
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
	EditorUpdate();
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
	// LoadUnityData(L"1_HN_Scene2");
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
	if (m_navMesh)
	{
		return m_navMesh->FindPath(_start, _end, _size);
	}
	return Vector3::Zero;
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
		if (e->m_parent.expired() && !e->m_isDead)
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
#ifndef EDITOR_MODE
	Start();
#endif // EDITOR_MODE
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

void Truth::Scene::LoadUnityData(const std::wstring& _path)
{
	if (_path.empty())
	{
		return;
	}



	auto gp = m_managers.lock()->Graphics();

	std::wstring mapPath = L"../Resources/MapData/" + _path + L"/";
	std::wstring assetPath = L"MapData/" + _path + L"/";
	std::filesystem::path p(mapPath);
	if (!std::filesystem::exists(p))
	{
		return;
	}

	/// read map data
	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(mapPath + L"Data.map", FileMode::Read);
	size_t objCount = file->Read<size_t>();

	m_mapEntity.resize(objCount);

	for (size_t i = 0; i < objCount; ++i)
	{
		m_mapEntity[i] = std::make_shared<Entity>(m_managers.lock());
		int32 parent = file->Read<int32>();
		std::string name = file->Read<std::string>();
		m_mapEntity[i]->m_name = name;
		m_mapEntity[i]->Initialize();


		if (parent != -1)
		{
			m_mapEntity[parent]->AddChild(m_mapEntity[i]);
			m_mapEntity[i]->m_parent = m_mapEntity[parent];
		}

		Vector3 pos;
		Vector3 sca;
		Quaternion rot;

		bool localPosChange[3] = { 0, };
		bool localScaleChange[3] = { 0, };
		bool localRotationChange = false;

		pos.x = file->Read<float>();
		pos.y = file->Read<float>();
		pos.z = file->Read<float>();

		rot.x = file->Read<float>();
		rot.y = file->Read<float>();
		rot.z = file->Read<float>();
		rot.w = file->Read<float>();

		sca.x = file->Read<float>();
		sca.y = file->Read<float>();
		sca.z = file->Read<float>();

		localPosChange[0] = file->Read<bool>();
		localPosChange[1] = file->Read<bool>();
		localPosChange[2] = file->Read<bool>();

		localScaleChange[0] = file->Read<bool>();
		localScaleChange[1] = file->Read<bool>();
		localScaleChange[2] = file->Read<bool>();

		localRotationChange = file->Read<bool>();

		// read Collider Data
		bool isCollider = file->Read<bool>();
		if (isCollider)
		{
			int32 type = file->Read<int32>();
			std::shared_ptr<Collider> coll;

			switch (type)
			{
			case 1:
			{
				Vector3 size = file->Read<Vector3>();
				Vector3 center = file->Read<Vector3>();
				coll = std::make_shared<BoxCollider>(center, size, false);
				break;
			}
			// 			case 2:
			// 			{
			// 				coll = std::make_shared<SphereCollider>(size, center);
			// 				break;
			// 			}
			// 			case 3:
			// 			{
			// 				coll = std::make_shared<CapsuleCollider>(size, center);
			// 				break;
			// 			}
			case 4:
			{
				coll = std::make_shared<MeshCollider>("MapData/1_HN_Scene2/" + name);
				break;
			}
			default:
				break;
			}

			if (coll)
			{
				m_mapEntity[i]->AddComponent(coll);
			}
		}

		// read Mesh Data
		bool isMesh = file->Read<bool>();
		if (isMesh)
		{
			std::string meshpath = file->Read<std::string>();

			std::filesystem::path mp(meshpath);

			size_t matCount = file->Read<size_t>();
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(assetPath + mp.filename().replace_extension("").generic_wstring());
			m_mapEntity[i]->AddComponent(mesh);
			mesh->SetMesh();

			for (size_t j = 0; j < matCount; ++j)
			{
				std::string matName = file->Read<std::string>();
				mesh->SetMaterialByIndex(j, matName);
			}

			mesh->SetStatic(true);
			Matrix mtm = mesh->GetMeshLocalTM();

			Vector3 mPos;
			Vector3 mSca;
			Quaternion mRot;

			mtm.Decompose(mSca, mRot, mPos);

			if (!localPosChange[0])
				pos.x = mPos.x;
			if (!localPosChange[1])
				pos.y = mPos.y;
			if (!localPosChange[2])
				pos.z = mPos.z;

			if (!localScaleChange[0])
				sca.x = mSca.x;
			if (!localScaleChange[1])
				sca.y = mSca.y;
			if (!localScaleChange[2])
				sca.z = mSca.z;

			if (!localRotationChange)
			{
				rot = mRot;
			}
		}

		// read Light Data
		bool isLight = file->Read<bool>();
		if (isLight)
		{
			file->Read<uint32>();
			file->Read<float>();
			file->Read<float>();
			file->Read<float>();
			file->Read<float>();
			file->Read<float>();
			file->Read<float>();
			file->Read<float>();
		}

		Matrix ltm = Matrix::CreateScale(sca);
		ltm *= Matrix::CreateFromQuaternion(rot);
		ltm *= Matrix::CreateTranslation(pos);

		Matrix flipYZ = Matrix::Identity;
		flipYZ.m[0][0] = -1.f;

		Matrix flipXY = Matrix::Identity;
		flipXY.m[2][2] = -1.f;

		m_mapEntity[i]->SetLocalTM(flipYZ * flipXY * ltm);
	}

	auto comp = [](std::shared_ptr<Entity> _a, std::shared_ptr<Entity> _b) -> bool
		{
			return _a->m_name > _b->m_name;
		};

	sort(m_mapEntity.begin(), m_mapEntity.end(), comp);

	// gp->BakeStaticMesh();

	for (auto& e : m_mapEntity)
	{
		e->Initialize();
		e->ApplyTransform();
		e->Awake();
		e->Start();
	}
}

