#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Entity.h"
/// <summary>
/// ������ ������ ���ư��� ��
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

		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	public:
		PROPERTY(name);
		std::string m_name;

	protected:
		PROPERTY(entities);
		std::vector<std::shared_ptr<Entity>> m_entities;

		std::weak_ptr<Managers> m_managers;

	private:
		std::queue<int> m_beginDestroy;
		std::queue<std::shared_ptr<Entity>> m_finishDestroy;

		std::queue<std::shared_ptr<Entity>> m_awakedEntity;
		std::queue<std::shared_ptr<Entity>> m_startedEntity;
		std::queue<std::shared_ptr<Entity>> m_watingEntity;
		
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

		void Update();
		void ApplyTransform();

		void Start();

		void Enter();
		void Exit();

		void ClearEntity();
	};

	template<class Archive>
	void Truth::Scene::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& (m_name);
		_ar& (m_entities);
	}
}
