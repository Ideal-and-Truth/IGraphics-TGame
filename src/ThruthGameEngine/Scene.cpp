#include "Scene.h"
#include "Entity.h"
#include "Managers.h"
#include "EventManager.h"
#include "GraphicsManager.h"
#include "NavMeshGenerater.h"
#include "PhysicsManager.h"
#include "TFileUtils.h"
#include "ISpotLight.h"
#include "Material.h"
#include "IMesh.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "Mesh.h"
#include "PointLight.h"
#include "DirectionLight.h"
#include "SpotLight.h"
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
	for (auto& m : m_mapEntity)
	{
		DeleteEntity(m);
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
	LoadUnityData(m_mapPath);
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
	return _end;
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
	for (auto& me : m_mapEntity)
	{
		DeleteEntity(me);
	}
	m_mapEntity.clear();
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

void Truth::Scene::ResizeWindow()
{
	for (auto& e : m_entities)
	{
		e->ResizeWindow();
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
	if (m_navMesh)
	{
		m_navMesh->Destroy();
		m_navMesh = nullptr;
	}
	for (auto& e : m_mapEntity)
	{
		e->Destroy();
		e.reset();
		e = nullptr;
	}
	for (auto& e : m_entities)
	{
		e->Destroy();
		e.reset();
		e = nullptr;
	}

	m_mapEntity.clear();
	m_entities.clear();
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
		return;

	auto gp = m_managers.lock()->Graphics();

	std::wstring mapPath = L"../Resources/MapData/" + _path + L"/";
	std::wstring assetPath = L"MapData/" + _path + L"/";
	std::filesystem::path p(mapPath);
	if (!std::filesystem::exists(p))
	{
		return;
	}

	/// read map data
	std::shared_ptr<TFileUtils> file = std::make_shared<TFileUtils>();
	file->Open(mapPath + L"Data.map", FileMode::Read);
	size_t objCount = file->Read<size_t>();

	m_mapEntity.resize(objCount);

	const static std::vector<Vector3> boxPoints =
	{
		{ -0.5, -0.5, -0.5 },
		{ 0.5, -0.5, -0.5 },
		{ 0.5, 0.5, -0.5 },
		{ -0.5, 0.5, -0.5 },
		{ -0.5, -0.5, 0.5 },
		{ 0.5, -0.5, 0.5 },
		{ 0.5, 0.5, 0.5 },
		{ -0.5, 0.5, 0.5 }
	};
	const static std::vector<uint32> boxIndices =
	{
		 0, 1, 2,
		 0, 2, 3,
		 4, 5, 6,
		 4, 6, 7,
		 0, 1, 5,
		 0, 5, 4,
		 1, 2, 6,
		 0, 6, 5,
		 2, 3, 7,
		 2, 7, 6,
		 3, 0, 4,
		 3, 4, 7
	};

	std::vector<float> vPositions;
	std::vector<uint32> vIndices;

	if (m_useNavMesh)
		m_navMesh = std::make_shared<NavMeshGenerater>();

	for (size_t i = 0; i < objCount; ++i)
	{
		/// read base data
		m_mapEntity[i] = std::make_shared<Entity>(m_managers.lock());
		int32 parent = file->Read<int32>();
		std::string name = file->Read<std::string>();
		m_mapEntity[i]->m_isStatic = true;
		m_mapEntity[i]->m_name = name;
		m_mapEntity[i]->Initialize();

		std::vector<float> vertexPosition;
		std::vector<uint32> indices;
		bool isBoxCollider = false;
		bool isSphereCollider = false;
		bool isCapsuleCollider = false;
		bool isMeshCollider = false;

		if (parent != -1)
		{
			m_mapEntity[parent]->AddChild(m_mapEntity[i]);
			m_mapEntity[i]->m_parent = m_mapEntity[parent];
		}

		/// read TM Dirty
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

		/// read Collider Data
		bool isCollider = file->Read<bool>();
		if (isCollider)
		{
			int32 type = file->Read<int32>();
			std::shared_ptr<Collider> coll;

			switch (type)
			{
			case 1:
			{
				isBoxCollider = true;
				Vector3 size = file->Read<Vector3>();
				Vector3 center = file->Read<Vector3>();
				coll = std::make_shared<BoxCollider>(center, size, false);
				break;
			}
			case 2:
			{
				isSphereCollider = true;
				Vector3 size = file->Read<Vector3>();
				Vector3 center = file->Read<Vector3>();
				coll = std::make_shared<SphereCollider>(center, size.x, false);
				break;
			}
			case 3:
			{
				isCapsuleCollider = true;
				Vector3 size = file->Read<Vector3>();
				Vector3 center = file->Read<Vector3>();
				coll = std::make_shared<CapsuleCollider>(center, size.x, size.y);
				break;
			}
			case 4:
			{
				isMeshCollider = true;
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

		/// read Mesh Data
		bool isMesh = file->Read<bool>();
		if (isMesh)
		{
			std::string meshpath = file->Read<std::string>();

			std::filesystem::path mp(meshpath);

			size_t matCount = file->Read<size_t>();
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(assetPath + mp.filename().replace_extension("").generic_wstring());
			m_mapEntity[i]->AddComponent(mesh);
			mesh->SetMesh();
			if (name == "50292")
			{
				int a = 1;
			}
			for (size_t j = 0; j < matCount; ++j)
			{
				std::string matName = file->Read<std::string>();
				mesh->SetMaterialByIndex(static_cast<uint32>(j), matName);
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

		/// set local TM
		Matrix ltm = Matrix::CreateScale(sca);
		ltm *= Matrix::CreateFromQuaternion(rot);
		ltm *= Matrix::CreateTranslation(pos);
		Matrix flipYZ = Matrix::Identity;
		flipYZ.m[0][0] = -1.f;

		Matrix flipXY = Matrix::Identity;
		flipXY.m[2][2] = -1.f;

		ltm = flipYZ * flipXY * ltm;

		m_mapEntity[i]->SetLocalTM(ltm);
		if (isMesh)
		{
			m_mapEntity[i]->GetComponent<Mesh>().lock()->SetMeshTransformMatrix(ltm);
		}
		/// create nav mesh data
		if (isBoxCollider)
		{
			size_t offset = vPositions.size() / 3;
			for (const auto& i : boxIndices)
			{
				vIndices.push_back(i + static_cast<uint32>(offset));
			}

			for (const auto& v : boxPoints)
			{
				Vector3 finalPosition = Vector3::Transform(v, ltm);
				vPositions.push_back(finalPosition.x);
				vPositions.push_back(finalPosition.y);
				vPositions.push_back(finalPosition.z);
			}
		}
		else if (isMeshCollider)
		{
			size_t offset = vPositions.size() / 3;
			std::shared_ptr<TFileUtils> posFile = std::make_shared<TFileUtils>();
			fs::path posFilePath = "../Resources/Models/MapData/1_HN_Scene2/";
			posFilePath /= name + ".pos";
			posFile->Open(posFilePath.generic_wstring(), FileMode::Read);

			uint32 meshSize = posFile->Read<uint32>();
			uint32 vertexSize = posFile->Read<uint32>();
			for (uint32 i = 0; i < vertexSize; i++)
			{
				Vector3 v;
				v.x = posFile->Read<float>();
				v.y = posFile->Read<float>();
				v.z = posFile->Read<float>();
				Vector3 finalPosition = Vector3::Transform(v, ltm);

				vPositions.push_back(finalPosition.x);
				vPositions.push_back(finalPosition.y);
				vPositions.push_back(finalPosition.z);
			}

			uint32 indexSize = posFile->Read<uint32>();
			for (uint32 i = 0; i < indexSize; i++)
			{
				vIndices.push_back(static_cast<uint32>(offset) + posFile->Read<uint32>());
			}
		}

		/// read Light Data
		bool isLight = file->Read<bool>();
		if (isLight)
		{
			uint32 lightType = file->Read<uint32>();
			float intensity = file->Read<float>();
			// intensity *= 0.5;
			Color lightColor;
			lightColor.x = file->Read<float>();
			lightColor.y = file->Read<float>();
			lightColor.z = file->Read<float>();
			lightColor.w = file->Read<float>();
			float range = file->Read<float>();
			float angle = file->Read<float>();

			Matrix rotMat = Matrix::CreateFromQuaternion(rot);
			Vector3 dir = { 0.0f, 0.0f, 1.0f };
			dir = Vector3::Transform(dir, rotMat);
			switch (lightType)
			{
			case 0:
			{
				std::shared_ptr<SpotLight> light = std::make_shared<SpotLight>();
				light->m_isRendering = true;
				light->m_position = pos;
				light->m_direction = dir;
				light->m_angle = angle;
				light->m_range = range;
				light->m_intensity = intensity * 1;
				light->m_softness = 5;
				light->m_lightColor = lightColor;
				m_mapEntity[i]->AddComponent(light);
				break;
			}
			case 1:
			{
				std::shared_ptr<DirectionLight> light = std::make_shared<DirectionLight>();
				light->m_isRendering = true;
				light->m_direction = dir;
				light->m_intensity = intensity;
				light->m_diffuseColor = lightColor;
				m_mapEntity[i]->AddComponent(light);
				break;
			}
			case 2:
			{
				std::shared_ptr<PointLight> light = std::make_shared<PointLight>();
				light->m_isRendering = true;
				light->m_position = pos;
				light->m_radius = range;
				light->m_intensity = intensity * 1;
				light->m_lightColor = lightColor;
				m_mapEntity[i]->AddComponent(light);
				break;
			}
			default:
				break;
			}
		}
	}

	/// create nav mesh
	if (m_useNavMesh)
		m_navMesh->Initalize(vPositions, vIndices);

	for (auto& e : m_mapEntity)
	{
		e->Initialize();
		e->ApplyTransform();
		e->Awake();
		e->Start();
	}
}

