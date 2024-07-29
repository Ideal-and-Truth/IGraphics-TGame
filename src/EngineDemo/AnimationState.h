#pragma once
#include "Component.h"

class AnimationState abstract
{
public:
	Truth::Entity* m_owner;
	Truth::Component* m_animator;


	AnimationState(Truth::Component* animator)
		: m_animator(animator)
	{
		m_owner = m_animator->GetOwner().lock().get();
	}

public:
	virtual void OnStateEnter() {}
	virtual void OnStateUpdate() {}
	virtual void OnStateExit() {}



	const Property* GetProperty(const std::string& name);

	template <typename T>
	void SetProperty(const std::string name, const T& value);
};

template <typename T>
void AnimationState::SetProperty(const std::string name, const T& value)
{
	m_animator->GetTypeInfo().GetProperty(name.c_str())->Set(m_animator, value);
}