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
		static uint16 m_entityCount;
		uint16 m_ID;
		PROPERTY(name)
			std::string m_name;
		std::shared_ptr<Managers> m_manager;

	public:
		// key ���� ��� type id �� ���� �����Ѵ�.
		PROPERTY(components)
			std::map<size_t, std::vector<std::shared_ptr<Component>>> m_components;

		PROPERTY(componentsTest)
			std::vector<std::shared_ptr<Component>> m_componentsTest;

		Entity();
		virtual ~Entity();
		virtual void Initailize();

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
		std::shared_ptr<C> component = std::make_shared<C>(m_manager);

		component->SetOwner(shared_from_this());

		// ���� �ߺ� ������ ������Ʈ���
		if (component->CanMultiple())
		{
			m_components[C::m_typeID].push_back(component);
			m_componentsTest.push_back(component);
			return component;
		}
		// �ߺ� �Ұ����� ������Ʈ���
		else if (!component->CanMultiple() && m_components[C::m_typeID].size() == 0)
		{
			m_components[C::m_typeID].push_back(component);
			return component;
		}
		else
		{
			return std::reinterpret_pointer_cast<C>(m_components[C::m_typeID][0]);
		}
	}

	template<typename C, typename... Args, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
	std::shared_ptr<C> Entity::AddComponent(Args... _args)
	{
		// �ϴ� �����
		// Ÿ�� �̸� ��������
		std::shared_ptr<C> component = std::make_shared<C>(m_manager, _args...);

		component->SetOwner(shared_from_this());

		// ���� �ߺ� ������ ������Ʈ���
		if (component->CanMultiple())
		{
			m_components[C::m_typeID].push_back(component);
			m_componentsTest.push_back(component);
			return component;
		}
		// �ߺ� �Ұ����� ������Ʈ���
		else if (!component->CanMultiple() && m_components[C::m_typeID].size() == 0)
		{
			m_components[C::m_typeID].push_back(component);
			return component;
		}
		else
		{
			return std::reinterpret_pointer_cast<C>(m_components[C::m_typeID][0]);
		}
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
		if (m_components.find(C::m_typeID) != m_components.end())
		{
			return std::reinterpret_pointer_cast<C>(m_components[C::m_typeID].front());
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
		if (m_components.find(C::m_typeID) != m_components.end())
		{
			for (auto& c : m_components[C::m_typeID])
			{
				result.push_back(std::reinterpret_pointer_cast<C>(c));
			}
			return result;
		}
		return result;
	}
}





