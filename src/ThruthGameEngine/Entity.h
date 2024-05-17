#pragma once
#include "Headers.h"

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
	class Entity abstract
		: public std::enable_shared_from_this<Entity>
	{
		GENERATE_CLASS_TYPE_INFO(Entity)

	protected:
		static uint32 m_entityCount;


		std::string m_tag;

		PROPERTY(ID);
		uint32 m_ID;
		PROPERTY(name);
		std::string m_name;
		std::shared_ptr<Managers> m_manager;

	public:
		// key 값의 경우 type id 를 통해 유추한다.
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

		std::vector<std::pair<Component*, const Method*>> m_onBevameVisible;
		std::vector<std::pair<Component*, const Method*>> m_onBevameInvisible;

		std::vector<std::pair<Component*, const Method*>> m_destroy;

		uint8 m_layer;

		Entity();
		virtual ~Entity();

		virtual void Initailize();

		void Awake();
		void Destroy();
		void Start();

		void OnCollisionEnter(const Collider* _other) const;

		template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::shared_ptr<C> AddComponent();
		template<typename C, typename... Args, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::shared_ptr<C> AddComponent(Args... _args);

		template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::weak_ptr<C> GetComponent();

		template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
		std::vector<std::weak_ptr<C>> GetComponents();

		void SetManager(std::shared_ptr<Managers> _val) { m_manager = _val; };

		std::string& GetName() { return m_name; };

	private:
	};

	/// template로 작성된 함수 목록

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

			auto met = component->GetTypeInfo().GetMethod("Initalize");
			if (met)
			{
				met->Invoke<void>(component.get());
			}
			m_components.push_back(component);

			const auto& mets = component->GetTypeInfo().GetMethods();

			for (const auto& m : mets)
			{
				std::string metName = m->GetName();

				auto p = std::make_pair(component.get(), m);

				if (metName == "OnCollisionEnter")
				{
					m_onCollisionEnter.push_back(p);
				}
				if (metName == "OnCollisionStay")
				{
					m_onCollisionStay.push_back(p);
				}
				if (metName == "OnCollisionExit")
				{
					m_onCollisionExit.push_back(p);
				}

				if (metName == "OnTriggerEnter")
				{
					m_onTriggerEnter.push_back(p);
				}
				if (metName == "OnTriggerStay")
				{
					m_onTriggerStay.push_back(p);
				}
				if (metName == "OnTriggerExit")
				{
					m_onTriggerExit.push_back(p);
				}

				if (metName == "Update")
				{
					m_update.push_back(p);
				}
				if (metName == "FixedUpdate")
				{
					m_fixedUpdate.push_back(p);
				}
				if (metName == "LateUpdate")
				{
					m_latedUpdate.push_back(p);
				}

				if (metName == "Destroy")
				{
					m_destroy.push_back(p);
				}
			}

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

			auto met = component->GetTypeInfo().GetMethod("Initalize");
			if (met)
			{
				met->Invoke<void>(component.get());
			}
			m_components.push_back(component);

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
