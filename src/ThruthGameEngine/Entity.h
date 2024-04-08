#pragma once
#include "Headers.h"

/// <summary>
/// ��� ��ƼƼ�� �θ� Ŭ����
/// �ϴ��� �̷��� �����غ���...
/// </summary>

class Component;

class Entity : public std::enable_shared_from_this<Entity>
{
private:
	std::string m_name;

public:
	std::shared_ptr<Entity> m_sharedThis;

	// key ���� ��� type id �� ���� �����Ѵ�.
	std::map<std::string, std::list<std::shared_ptr<Component>>> m_multipleComponents;
	std::map<std::string, std::shared_ptr<Component>> m_uniqueComponents;

	Entity();
	virtual ~Entity();

	virtual void Update(float4 _dt) abstract;
	virtual void Render() abstract;
	virtual void LateUpdate(float4 _dt) abstract;
	virtual void FixedUpdate(float4 _dt) abstract;

	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
	void AddComponent();

	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
	std::weak_ptr<Component> GetComponent();

	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
	std::list<std::shared_ptr<Component>> GetComponents();
};

template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
void Entity::AddComponent()
{
	// �ϴ� �����
	// Ÿ�� �̸� ��������
	std::shared_ptr<C> component = std::make_shared<C>();
	component->SetOwner(m_sharedThis);
	std::string tName = typeid(C).name();

	// ���� �ߺ� ������ ������Ʈ���
	if (component->CanMultiple())
	{
		// �� ������Ʈ�� ó���̶��
		if (m_multipleComponents.find(tName) == m_multipleComponents.end())
		{
			m_multipleComponents[tName] = std::list<std::shared_ptr<Component>>();
		}
		m_multipleComponents[tName].push_back(component);
	}
	// �ߺ� �Ұ����� ������Ʈ���
	else
	{
		// ������Ʈ�� ���ٸ�
		assert(m_uniqueComponents.find(tName) == m_uniqueComponents.end());
		m_uniqueComponents[tName] = component;
	}
}

template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
std::weak_ptr<Component> Entity::GetComponent()
{
	// Ÿ�� �̸� ��������
	std::string tName = typeid(C).name();
	if (m_multipleComponents.find(tName) != m_multipleComponents.end())
	{
		return m_multipleComponents[tName].front();
	}
	else if (m_uniqueComponents.find(tName) != m_uniqueComponents.end())
	{
		return m_uniqueComponents[tName];
	}
}

template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
std::list<std::shared_ptr<Component>> Entity::GetComponents()
{
	// Ÿ�� �̸� ��������
	std::string tName = typeid(C).name();
	if (m_multipleComponents.find(tName) != m_multipleComponents.end())
	{
		return m_multipleComponents[tName];
	}
	else if (m_uniqueComponents.find(tName) != m_uniqueComponents.end())
	{
		std::list<std::shared_ptr<Component>> result;
		result.push_back(m_uniqueComponents[tName]);
		return result;
	}
}

