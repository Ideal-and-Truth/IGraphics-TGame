#pragma once
#include "Headers.h"

/// <summary>
/// 모든 엔티티의 부모 클래스
/// 일단은 이렇게 구현해보자...
/// </summary>

class Component;
class ETransform;
class Managers;

class Entity abstract
	: public std::enable_shared_from_this<Entity>
{
	GENERATE_CLASS_TYPE_INFO(Entity)

protected:
	static uint16 m_entityCount;
	uint16 m_ID;
	std::string m_name;
	std::weak_ptr<Managers> m_manager;

public:
	// key 값의 경우 type id 를 통해 유추한다.
	std::map<size_t, std::vector<std::shared_ptr<Component>>> m_components;

	Entity();
	virtual ~Entity();
	virtual void Initailize();

	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
	void AddComponent();

	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
	std::weak_ptr<C> GetComponent();

	template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type* = nullptr>
	std::vector<std::weak_ptr<C>> GetComponents();

	void SetManager(std::weak_ptr<Managers> _val) { m_manager = _val; };

	std::string& GetName() { return m_name; };
};



/// template로 작성된 함수 목록

/// <summary>
/// 엔티티에 컴포넌트 추가
/// </summary>
/// <typeparam name="C">컴포넌트 타입</typeparam>
template<typename C, typename std::enable_if<std::is_base_of_v<Component, C>, C>::type*>
void Entity::AddComponent()
{
	// 일단 만든다
	// 타입 이름 가져오기
	std::shared_ptr<C> component = std::make_shared<C>();

	component->SetOwner(shared_from_this());
	component->SetManager(m_manager);
	component->Awake();

	// 만일 중복 가능한 컴포넌트라면
	if (component->CanMultiple())
	{
		m_components[C::m_typeID].push_back(component);
	}
	// 중복 불가능한 컴포넌트라면
	else if (!component->CanMultiple() && m_components[C::m_typeID].size() == 0)
	{
		m_components[C::m_typeID].push_back(component);
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
	if (m_components.find(C::m_typeID) != m_components.end())
	{
		return std::reinterpret_pointer_cast<C>(m_components[C::m_typeID].front());
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

