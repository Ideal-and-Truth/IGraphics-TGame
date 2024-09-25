#pragma once
#include "Component.h"
#include "AnimationState.h"

namespace Truth
{
	class SkinnedMesh;
}

class Enemy;
class EnemyController;
class PlayerAnimator;


class BossIdle
	: public AnimationState
{
private:


public:
	BossIdle(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class BossRun
	: public AnimationState
{
private:


public:
	BossRun(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class BossStrafe
	: public AnimationState
{
private:
	float m_sidePose;
	bool m_isChangePose;

public:
	BossStrafe(Truth::Component* animator)
		: AnimationState(animator)
		, m_sidePose(0.f)
		, m_isChangePose(false)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class BossDodge
	: public AnimationState
{
private:


public:
	BossDodge(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class BossAttackSwing
	: public AnimationState
{
private:


public:
	BossAttackSwing(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class BossAttackRunning
	: public AnimationState
{
private:


public:
	BossAttackRunning(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class BossAttackUpperCut
	: public AnimationState
{
private:


public:
	BossAttackUpperCut(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class BossAttackChargeCombo
	: public AnimationState
{
private:


public:
	BossAttackChargeCombo(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class BossAttackSmashGround
	: public AnimationState
{
private:


public:
	BossAttackSmashGround(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class BossAttackCharge
	: public AnimationState
{
private:


public:
	BossAttackCharge(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class BossAttackJumpSmashGround
	: public AnimationState
{
private:


public:
	BossAttackJumpSmashGround(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class BossAttackSpin
	: public AnimationState
{
private:


public:
	BossAttackSpin(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class BossAttackDoubleUpperCut
	: public AnimationState
{
private:
	bool m_isChangePose;

public:
	BossAttackDoubleUpperCut(Truth::Component* animator)
		: AnimationState(animator)
		, m_isChangePose(false)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};


// 애니메이터 없어서 임시로 만든 컴포넌트
class BossAnimator :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(BossAnimator);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	std::shared_ptr<Truth::SkinnedMesh> m_skinnedMesh;
	std::shared_ptr<Enemy> m_enemy;
	std::shared_ptr<EnemyController> m_enemyController;

	std::shared_ptr<PlayerAnimator> m_playerAnimator;


	/// <summary>
	///  상태 조절을 위한 것들
	/// </summary>
	/// <param name=""></param>
	/// 
	/// ----------------------------------------
	PROPERTY(isPursuit);
	bool m_isPursuit;

	PROPERTY(strafeMove);
	bool m_strafeMove;

	PROPERTY(isReturn);
	bool m_isReturn;

	PROPERTY(isInRange);
	bool m_isInRange;

	PROPERTY(attackSwing);
	bool m_attackSwing;

	PROPERTY(attackRunning);
	bool m_attackRunning;

	PROPERTY(attackUpperCut);
	bool m_attackUpperCut;

	PROPERTY(attackChargeCombo);
	bool m_attackChargeCombo;

	PROPERTY(attackSmashGround);
	bool m_attackSmashGround;

	PROPERTY(attackCharge);
	bool m_attackCharge;

	PROPERTY(attackJumpSmashGround);
	bool m_attackJumpSmashGround;

	PROPERTY(attackSpin);
	bool m_attackSpin;

	PROPERTY(attackDoubleUpperCut);
	bool m_attackDoubleUpperCut;

	PROPERTY(isDamage);
	bool m_isDamage;

	PROPERTY(isDown);
	bool m_isDown;

	PROPERTY(isDeath);
	bool m_isDeath;

	PROPERTY(attackCoolTime);
	float m_attackCoolTime;

	PROPERTY(sideMove);
	float m_sideMove;

	/// ----------------------------------------
	PROPERTY(passingTime);
	float m_passingTime;

	float m_lastHp;

	PROPERTY(isAnimationEnd);
	bool m_isAnimationEnd;

	PROPERTY(animationStateMap);
	std::map<std::string, AnimationState*> m_animationStateMap;

	AnimationState* m_currentState;

	PROPERTY(currentFrame);
	int m_currentFrame;

	PROPERTY(hitStopTime);
	float m_hitStopTime;

private:
	int RandomNumber(int _min, int _max);

public:
	BossAnimator();
	virtual ~BossAnimator();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();


	void SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished);

	void SetAnimationSpeed(float speed);

	void SetAnimationPlay(bool playStop);

	void ChangeState(std::string stateName);

	void SetEnemyDamage(float damage);
};

template<class Archive>
void BossAnimator::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(BossAnimator)