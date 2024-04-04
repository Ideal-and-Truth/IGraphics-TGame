#pragma once
#include "Headers.h"

/// <summary>
/// ��� ��ƼƼ�� �θ� Ŭ����
/// �ϴ��� �̷��� �����غ���...
/// </summary>

class Component;

class Entity
{
private:
	std::string m_name;

	// key ���� ��� type id �� ���� �����Ѵ�.
	std::map<std::string, std::list<std::shared_ptr<Component>>> m_multipleComponents;
	std::map<std::string, std::shared_ptr<Component>> m_uniqueComponents;

public:
	Entity();
	virtual ~Entity();

	virtual void Update(float4 _dt) abstract;
	virtual void Render() abstract;
	virtual void LateUpdate(float4 _dt) abstract;
	virtual void FixedUpdate(float4 _dt) abstract;

	template<typename C>
	void AddComponent();
};

template<typename C>
void Entity::AddComponent()
{
	// �ϴ� �����
	std::shared_ptr<C> component = std::make_shared<C>();
	// Ÿ�� �̸� ��������
	std::string tName = typeid(component).name();

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

