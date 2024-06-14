#pragma once
#include "Headers.h"
#include "Component.h"

/// <summary>
/// 모든 엔티티의 부모 클래스
/// 일단은 이렇게 구현해보자...
/// </summary>
namespace Truth
{
	class Component;
	class Transform;
	class Managers;
	class Collider;
}

namespace Truth
{
	class Entity final
		: public std::enable_shared_from_this<Entity>
	{
		GENERATE_CLASS_TYPE_INFO(Entity);

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& ar, const unsigned int file_version);

	protected:
		static uint32 m_entityCount;


		std::string m_tag;

		std::weak_ptr<Managers> m_manager;

	public:
		int32 m_index;

		PROPERTY(ID);
		uint32 m_ID;

		// key 값의 경우 type id 를 통해 유추한다.
		PROPERTY(layer);
		uint8 m_layer;

		PROPERTY(name);
		std::string m_name;

		PROPERTY(components);
		std::vector<std::shared_ptr<Component>> m_components;

		std::vector<std::pair<Component*, const Method*>> m_onCollisionEnter;
		std::vector<std::pair<Component*, const Method*>> m_onCollisionStay;
		std::vector<std::pair<Component*, const Method*>> m_onCollisionExit;

		std::vector<std::pair<Component*, const Method*>> m_onTriggerEnter;
		std::vector<std::pair<Component*, const Method*>> m_onTriggerStay;
		std::vector<std::pair<Component*, const Method*>> m_onTriggerExit;

		std::vector<std::pair<Component*, const Method*>> m_update;
		std::vector<std::pair<Component*, const Method*>> m_fixedUpdate;
		std::vector<std::pair<Component*, const Method*>> m_latedUpdate;

		std::vector<std::pair<Component*, const Method*>> m_onBecomeVisible;
		std::vector<std::pair<Component*, const Method*>> m_onBecomeInvisible;

		std::vector<std::pair<Component*, const Method*>> m_destroy;
		std::vector<std::pair<Component*, const Method*>> m_applyTransform;

		std::shared_ptr<Transform> m_transform;

	public:
		Entity(std::shared_ptr<Managers> _mangers);
		Entity() = default;
		~Entity();

		void Initailize();

		void SetPosition(const Vector3& _pos) const;
		void SetScale(const Vector3& _scale) const;

		const Vector3& GetPosition() const;
		const Quaternion& GetRotation() const;
		const Vector3& GetScale() const;

		const Matrix& GetWorldTM() const;
		void SetWorldTM(const Matrix& _tm);

		void ApplyTransform() const;

		void Awake();
		void Destroy();
		void Start();

		void Update();

		void OnCollisionEnter(Collider* _other);
		void OnCollisionStay(Collider* _other);
		void OnCollisionExit(Collider* _other);

		void OnTriggerEnter(Collider* _other);
		void OnTriggerStay(Collider* _other);
		void OnTriggerExit(Collider* _other);

		template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::shared_ptr<C> AddComponent();
		template<typename C, typename... Args, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::shared_ptr<C> AddComponent(Args... _args);

		template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::weak_ptr<C> GetComponent();

		template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::vector<std::weak_ptr<C>> GetComponents();

		void SetManager(std::weak_ptr<Managers> _val) { m_manager = _val; };

		std::string& GetName() { return m_name; };


	private:

		void ApplyComponent(std::shared_ptr<Component> _c);

	};

	/// template로 작성된 함수 목록

	template<class Archive>
	void Entity::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& m_name;
		_ar& m_ID;
		_ar& m_layer;
		_ar& m_components;
	}

	/// <summary>
	/// 엔티티에 컴포넌트 추가
	/// </summary>
	/// <typeparam name="C">컴포넌트 타입</typeparam>
	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::shared_ptr<C> Entity::AddComponent()
	{
		if (GetComponent<C>().expired() || GetComponent<C>().lock()->CanMultiple())
		{
			std::shared_ptr<C> component = nullptr;
			component = std::make_shared<C>();
			component->SetManager(m_manager);
			component->SetOwner(shared_from_this());
			m_components.push_back(component);
			component->Initalize();
			ApplyComponent(component);

			return component;
		}
		else
		{
			return nullptr;
		}
	}

	template<typename C, typename... Args, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::shared_ptr<C> Entity::AddComponent(Args... _args)
	{
		if (GetComponent<C>().expired() || GetComponent<C>().lock()->CanMultiple())
		{
			std::shared_ptr<C> component = nullptr;
			component = std::make_shared<C>(_args...);
			component->SetManager(m_manager);
			component->SetOwner(shared_from_this());
			m_components.push_back(component);
			component->Initalize();
			ApplyComponent(component);

			return component;
		}
		else
		{
			return nullptr;
		}
	}

	/// <summary>
	/// 컴포넌트 가져오기
	/// 여려개가 있는 컴포넌트의 경우 가장 바깥에 있는 하나만 반환 
	/// </summary>
	/// <typeparam name="C">컴포넌트 타입</typeparam>
	/// <returns>컴포넌트</returns>
	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::weak_ptr<C> Entity::GetComponent()
	{
		for (auto& c : m_components)
		{
			auto component = ::Cast<C, Component>(c);
			if (component != nullptr)
			{
				return component;
			}
		}
		return std::weak_ptr<C>();
	}

	/// <summary>
	/// 컴포넌트들 가져오기
	/// 한개인 경우 크기가 하나인 리스트로 돌려준다.
	/// </summary>
	/// <typeparam name="C">컴포넌트 타입</typeparam>
	/// <returns>컴포넌트 리스트</returns>
	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::vector<std::weak_ptr<C>> Entity::GetComponents()
	{
		std::vector<std::weak_ptr<C>> result;
		for (auto& c : m_components)
		{
			auto component = ::Cast<C, Component>(c);
			if (component != nullptr)
			{
				result.push_back(component);
			}
		}
		return result;
	}
}
