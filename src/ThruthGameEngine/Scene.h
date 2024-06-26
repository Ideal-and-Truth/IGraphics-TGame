#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Entity.h"
/// <summary>
/// 게임이 실제로 돌아가는 씬
/// </summary>
namespace Truth
{
	class Managers;

	class Scene final
		: public EventHandler
	{
		GENERATE_CLASS_TYPE_INFO(Scene);

	private:
		friend class boost::serialization::access;
		friend class EditorUI;

		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	public:
		PROPERTY(name);
		std::string m_name;



	private:
		typedef std::vector<std::shared_ptr<Entity>> EntityVector;
		PROPERTY(entities);
		EntityVector m_entities;
#ifdef _DEBUG
		EntityVector m_rootEntities;
#endif // _DEBUG

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

		// 후에 씬에 오브젝트가 추가되어 다음 프레임부터 Update 시작
		void AddEntity(std::shared_ptr<Entity> _p);
		void CreateEntity(std::shared_ptr<Entity> _p);

		void DeleteEntity(uint32 _index);
		void DeleteEntity(std::shared_ptr<Entity> _p);

		void Initalize(std::weak_ptr<Managers> _manager);

#ifdef _DEBUG
		void EditorUpdate();
#endif // _DEBUG

		void Update();
		void FixedUpdate();
		void LateUpdate();

		void ApplyTransform();

		void Start();

		void Enter();
		void Exit();

		void ClearEntity();
	};

	template<class Archive>
	void Truth::Scene::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& m_name;
		_ar& m_entities;
	}

	template<class Archive>
	void Truth::Scene::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& m_name;
		_ar& m_entities;
	}
}
