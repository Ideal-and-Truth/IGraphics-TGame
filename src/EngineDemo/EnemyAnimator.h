#pragma once
#include "Component.h"
#include "AnimationState.h"

namespace Truth
{
	class SkinnedMesh;
}

class Enemy;
class EnemyController;


class EnemyIdle
	: public AnimationState
{
private:


public:
	EnemyIdle(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class EnemyChase
	: public AnimationState
{
private:
	bool m_isChangePose;
	bool m_isBackStep;

public:
	EnemyChase(Truth::Component* animator)
		: AnimationState(animator)
		 , m_isChangePose(false)
		 , m_isBackStep(false)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class EnemyReturn
	: public AnimationState
{
private:


public:
	EnemyReturn(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class EnemyAttackReady
	: public AnimationState
{
private:
	float m_sidePose;
	bool m_isChangePose;

public:
	EnemyAttackReady(Truth::Component* animator)
		: AnimationState(animator)
		, m_sidePose(0.f)
		, m_isChangePose(false)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class EnemyAttack
	: public AnimationState
{
private:


public:
	EnemyAttack(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class EnemyParriableAttack
	: public AnimationState
{
private:


public:
	EnemyParriableAttack(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class EnemyHit
	: public AnimationState
{
private:


public:
	EnemyHit(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class EnemyParried
	: public AnimationState
{
private:


public:
	EnemyParried(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};

class EnemyDeath
	: public AnimationState
{
private:


public:
	EnemyDeath(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};


// 애니메이터 없어서 임시로 만든 컴포넌트
class EnemyAnimator :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(EnemyAnimator);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	std::shared_ptr<Truth::SkinnedMesh> m_skinnedMesh;
	std::shared_ptr<Enemy> m_enemy;
	std::shared_ptr<EnemyController> m_enemyController;


	/// <summary>
	///  상태 조절을 위한 것들
	/// </summary>
	/// <param name=""></param>
	/// 
	/// ----------------------------------------

	PROPERTY(isChase);
	bool m_isChase;

	PROPERTY(isComeBack);
	bool m_isComeBack;

	PROPERTY(isAttackReady);
	bool m_isAttackReady;

	PROPERTY(isAttack);
	bool m_isAttack;

	PROPERTY(isParryAttack);
	bool m_isParryAttack;

	PROPERTY(isHit);
	bool m_isHit;

	PROPERTY(isParried);
	bool m_isParried;

	PROPERTY(isDead);
	bool m_isDead;

	PROPERTY(sideMove);
	float m_sideMove;

	PROPERTY(isBackStep);
	bool m_isBackStep;

	PROPERTY(isAttacking);
	bool m_isAttacking;
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

private:
	int RandomNumber(int _min, int _max);

public:
	EnemyAnimator();
	virtual ~EnemyAnimator();

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
void EnemyAnimator::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(EnemyAnimator)