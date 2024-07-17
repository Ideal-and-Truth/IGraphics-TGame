#pragma once
#include "Component.h"

namespace Truth
{
	class SkinnedMesh;
}

class PlayerController;

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
	virtual void Initialize() {}
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

class PlayerIdle
	: public AnimationState
{
private:
	PlayerController* m_pc;
	const Property* m_forward;
	const Property* m_side;



public:
	PlayerIdle(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void Initialize() override;
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class PlayerRun
	: public AnimationState
{
private:
	PlayerController* m_pc;
	const Property* m_forward;
	const Property* m_side;



public:
	PlayerRun(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void Initialize() override;
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

// 애니메이터 없어서 임시로 만든 컴포넌트
class PlayerAnimator :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(PlayerAnimator);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	std::shared_ptr<Truth::SkinnedMesh> m_skinnedMesh;


	PROPERTY(isMove);
	bool m_isMove;


	PROPERTY(isAnimationEnd);
	bool m_isAnimationEnd;

	std::map<std::string, AnimationState*> m_animationStateMap;
	AnimationState* m_currentState;

public:
	PlayerAnimator();
	virtual ~PlayerAnimator();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();


	void SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished);

	void ChangeState(AnimationState* state);
};

template<class Archive>
void PlayerAnimator::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerAnimator)