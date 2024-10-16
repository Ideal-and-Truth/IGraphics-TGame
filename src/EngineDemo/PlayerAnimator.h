#pragma once
#include "Component.h"
#include "AnimationState.h"
#include "Collider.h"


namespace Truth
{
	class SkinnedMesh;
}

class Player;
class PlayerController;
class PlayerCamera;


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
	bool isReset = false;

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
	bool isReset = false;

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
	bool isReset = false;

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
	bool isReset = false;

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

class NormalAttack6
	: public AnimationState
{
private:
	bool isReset = false;

public:
	NormalAttack6(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class ComboReady
	: public AnimationState
{
private:

public:
	ComboReady(Truth::Component* animator)
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
	bool isReset = false;

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

class ChargedAttack2
	: public AnimationState
{
private:
	bool isReset = false;

public:
	ChargedAttack2(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class ChargedAttack3
	: public AnimationState
{
private:
	bool isReset = false;

public:
	ChargedAttack3(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class ChargedAttack4
	: public AnimationState
{
private:
	bool isReset = false;

public:
	ChargedAttack4(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class ChargedAttack5
	: public AnimationState
{
private:
	bool isReset = false;

public:
	ChargedAttack5(Truth::Component* animator)
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
	bool isHit = false;

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

class PlayerDodgeAttack
	: public AnimationState
{
private:

public:
	PlayerDodgeAttack(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};


// �ִϸ����� ��� �ӽ÷� ���� ������Ʈ
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
	std::shared_ptr<PlayerCamera> m_playerCamera;


	/// <summary>
	///  ���� ������ ���� �͵�
	/// </summary>
	/// <param name=""></param>
	/// 
	/// ----------------------------------------
	PROPERTY(isRun);
	bool m_isRun;

	PROPERTY(isAttack);
	bool m_isAttack;

	PROPERTY(isNormalAttack);
	bool m_isNormalAttack;

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

	PROPERTY(isLockOn);
	bool m_isLockOn;

	PROPERTY(isComboReady);
	bool m_isComboReady;

	PROPERTY(isDead);
	bool m_isDead;

	PROPERTY(downAttack);
	bool m_downAttack;

	PROPERTY(chargedReady);
	bool m_chargedReady;

	PROPERTY(forwardInput);
	float m_forwardInput;

	PROPERTY(sideInput);
	float m_sideInput;

	/// ----------------------------------------

	// ����Ʈ������ �ӽ÷�
	PROPERTY(normalAttack1);
	bool m_normalAttack1;

	PROPERTY(normalAttack2);
	bool m_normalAttack2;

	PROPERTY(normalAttack3);
	bool m_normalAttack3;

	PROPERTY(normalAttack4);
	bool m_normalAttack4;

	PROPERTY(normalAttack6);
	bool m_normalAttack6;
	
	float m_lastHp;

	float m_passingTime;

	float m_chargedTime;

	PROPERTY(hitStopTime);
	float m_hitStopTime;

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

	METHOD(OnTriggerEnter);
	void OnTriggerEnter(Truth::Collider* _other);

	METHOD(OnCollisionEnter);
	void OnCollisionEnter(Truth::Collider* _other);

	void SetImpulse(float power, bool needRot);

	void SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished);

	void SetAnimationSpeed(float speed);

	void ChangeState(std::string stateName);

	void SetPlayerDamage(float damage);
};

template<class Archive>
void PlayerAnimator::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerAnimator)