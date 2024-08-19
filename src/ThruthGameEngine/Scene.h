#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Entity.h"

namespace Ideal
{
	class IMeshObject;
}

class EditorUI;

/// <summary>
/// ������ ������ ���ư��� ��
/// </summary>
namespace Truth
{
	class Managers;
	class NavMeshGenerater;

	class Scene final
		: public EventHandler
	{
		GENERATE_CLASS_TYPE_INFO(Scene);

	private:
		friend class boost::serialization::access;
		friend class LoadMapDataPopup;

		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	public:
		PROPERTY(name);
		std::string m_name;

		std::wstring m_mapPath;

		std::shared_ptr<NavMeshGenerater> m_navMesh;

		std::vector<std::shared_ptr<Ideal::IMeshObject>> m_mapMesh;
		std::vector<std::shared_ptr<Ideal::ILight>> m_mapLight;

	public:
		typedef std::vector<std::shared_ptr<Entity>> EntityVector;
		PROPERTY(entities);
		EntityVector m_entities;
		EntityVector m_rootEntities;

		std::weak_ptr<Managers> m_managers;  

	private:
		std::queue<std::shared_ptr<Entity>> m_beginDestroy;
		std::queue<std::shared_ptr<Entity>> m_finishDestroy;

		std::queue<std::shared_ptr<Entity>> m_awakedEntity;
		std::queue<std::shared_ptr<Entity>> m_startedEntity;

	public:
		Scene(std::shared_ptr<Managers> _managers);
		Scene() = default;
		virtual ~Scene();

		// �Ŀ� ���� ������Ʈ�� �߰��Ǿ� ���� �����Ӻ��� Update ����
		void AddEntity(std::shared_ptr<Entity> _p);
		void CreateEntity(std::shared_ptr<Entity> _p);

		void DeleteEntity(uint32 _index);
		void DeleteEntity(std::shared_ptr<Entity> _p);

		void Initalize(std::weak_ptr<Managers> _manager);

		void LoadEntity(std::shared_ptr<Entity> _entity);

		Vector3 FindPath(Vector3 _start, Vector3 _end, Vector3 _size) const;

		std::weak_ptr<Entity> FindEntity(std::string _name);


#ifdef EDITOR_MODE
		void EditorUpdate();
#endif // EDITOR_MODE

		void Update();
		void FixedUpdate();
		void LateUpdate();

		void ApplyTransform();

		void Start();

		void Enter();
		void Exit();

		void ClearEntity();

	private:
		void CreateMap(const std::wstring& _path);
		void ResetMapData();

	};

	template<class Archive>
	void Truth::Scene::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& m_name;
		_ar& m_rootEntities;
		_ar& m_mapPath;
	}

	template<class Archive>
	void Truth::Scene::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& m_name;
		_ar& m_rootEntities;
		if (file_version >= 2)
		{
			_ar& m_mapPath;
		}
	}
}
BOOST_CLASS_VERSION(Truth::Scene, 2)