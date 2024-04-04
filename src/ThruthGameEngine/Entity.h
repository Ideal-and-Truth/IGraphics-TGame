#pragma once
#include "Headers.h"

/// <summary>
/// 모든 엔티티의 부모 클래스
/// 일단은 이렇게 구현해보자...
/// </summary>

class Component;

class Entity
{
private:
	std::string m_name;

	// key 값의 경우 type id 를 통해 유추한다.
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
	// 일단 만든다
	std::shared_ptr<C> component = std::make_shared<C>();
	// 타입 이름 가져오기
	std::string tName = typeid(component).name();

	// 만일 중복 가능한 컴포넌트라면
	if (component->CanMultiple())
	{
		// 이 컴포넌트가 처음이라면
		if (m_multipleComponents.find(tName) == m_multipleComponents.end())
		{
			m_multipleComponents[tName] = std::list<std::shared_ptr<Component>>();
		}
		m_multipleComponents[tName].push_back(component);
	}
	// 중복 불가능한 컴포넌트라면
	else
	{
		// 컴포넌트가 없다면
		assert(m_uniqueComponents.find(tName) == m_uniqueComponents.end());
		m_uniqueComponents[tName] = component;
	}
}

