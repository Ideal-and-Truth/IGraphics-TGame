#pragma once
#include "Component.h"

namespace Truth
{
	class SkinnedMesh;
}

class PlayerController;
class Player;

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

class PlayerIdle
	: public AnimationState
{
private:


public:
	PlayerIdle(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class PlayerWalk
	: public AnimationState
{
private:


public:
	PlayerWalk(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class PlayerRun
	: public AnimationState
{
private:


public:
	PlayerRun(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class NormalAttack1
	: public AnimationState
{
private:

public:
	NormalAttack1(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class NormalAttack2
	: public AnimationState
{
private:

public:
	NormalAttack2(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class NormalAttack3
	: public AnimationState
{
private:

public:
	NormalAttack3(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class NormalAttack4
	: public AnimationState
{
private:

public:
	NormalAttack4(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class ChargedAttack1
	: public AnimationState
{
private:

public:
	ChargedAttack1(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class PlayerGuard
	: public AnimationState
{
private:

public:
	PlayerGuard(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class PlayerHit
	: public AnimationState
{
private:

public:
	PlayerHit(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class PlayerDodge
	: public AnimationState
{
private:

public:
	PlayerDodge(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
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
	std::shared_ptr<PlayerController> m_playerController;
	std::shared_ptr<Player> m_player;


	/// <summary>
	///  상태 조절을 위한 것들
	/// </summary>
	/// <param name=""></param>
	PROPERTY(isWalk);
	bool m_isWalk;

	PROPERTY(isRun);
	bool m_isRun;

	PROPERTY(isAttack);
	bool m_isAttack;

	PROPERTY(isChargedAttack);
	bool m_isChargedAttack;

	PROPERTY(isCharged);
	float m_isCharged;

	PROPERTY(isAttacking);
	bool m_isAttacking;

	PROPERTY(isGuard);
	bool m_isGuard;

	PROPERTY(isHit);
	bool m_isHit;

	PROPERTY(isDodge);
	bool m_isDodge;


	PROPERTY(isAnimationEnd);
	bool m_isAnimationEnd;

	PROPERTY(animationStateMap);
	std::map<std::string, AnimationState*> m_animationStateMap;

	AnimationState* m_currentState;

	PROPERTY(currentFrame);
	int m_currentFrame;

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

	void ChangeState(std::string stateName);

	void SetPlayerDamage(float damage);
};

template<class Archive>
void PlayerAnimator::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerAnimator)