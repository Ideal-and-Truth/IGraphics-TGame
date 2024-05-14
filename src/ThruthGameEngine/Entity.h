#pragma once
#include "Headers.h"

/// <summary>
/// ��� ��ƼƼ�� �θ� Ŭ����
/// �ϴ��� �̷��� �����غ���...
/// </summary>
namespace Truth
{
	class Component;
	class Transform;
	class Managers;
}

namespace Truth
{
	class Entity abstract
		: public std::enable_shared_from_this<Entity>
	{
		GENERATE_CLASS_TYPE_INFO(Entity)

	protected:
		static uint32 m_entityCount;

		PROPERTY(ID);
		uint32 m_ID;
		PROPERTY(name);
		std::string m_name;
		std::shared_ptr<Managers> m_manager;

	public:
		// key ���� ��� type id �� ���� �����Ѵ�.
		PROPERTY(components);
		std::vector<std::shared_ptr<Component>> m_components;

		Entity();
		virtual ~Entity();

		virtual void Initailize();

		void Awake();
		void Destroy();
		void Start();

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

	/// template�� �ۼ��� �Լ� ���

	/// <summary>
	/// ��ƼƼ�� ������Ʈ �߰�
	/// </summary>
	/// <typeparam name="C">������Ʈ Ÿ��</typeparam>
	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::shared_ptr<C> Entity::AddComponent()
	{
		// �ϴ� �����
		// Ÿ�� �̸� ��������
		std::shared_ptr<C> component = std::make_shared<C>();
		component->SetManager(m_manager);
		component->SetOwner(shared_from_this());
		// ���� �ߺ� ������ ������Ʈ���
		if (component->CanMultiple())
		{
			m_components.push_back(component);
			return component;
		}
		else
		{
			if (!GetComponent<C>().expired())
			{
				return nullptr;
			}
			m_components.push_back(component);
			return component;
		}
	}

	template<typename C, typename... Args, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::shared_ptr<C> Entity::AddComponent(Args... _args)
	{
		// �ϴ� �����
		// Ÿ�� �̸� ��������
		std::shared_ptr<C> component = std::make_shared<C>(_args...);
		component->SetManager(m_manager);
		component->SetOwner(shared_from_this());
		// ���� �ߺ� ������ ������Ʈ���
		if (component->CanMultiple())
		{
			m_components.push_back(component);
		}
		else
		{
			if (GetComponent<C>().expired())
			{
				m_components.push_back(component);
			}
			else
			{
				return nullptr;
			}
		}
		return component;
	}

	/// <summary>
	/// ������Ʈ ��������
	/// �������� �ִ� ������Ʈ�� ��� ���� �ٱ��� �ִ� �ϳ��� ��ȯ 
	/// </summary>
	/// <typeparam name="C">������Ʈ Ÿ��</typeparam>
	/// <returns>������Ʈ</returns>
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
	/// ������Ʈ�� ��������
	/// �Ѱ��� ��� ũ�Ⱑ �ϳ��� ����Ʈ�� �����ش�.
	/// </summary>
	/// <typeparam name="C">������Ʈ Ÿ��</typeparam>
	/// <returns>������Ʈ ����Ʈ</returns>
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
