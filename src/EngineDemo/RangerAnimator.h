#pragma once
#include "Component.h"
#include "AnimationState.h"

namespace Truth
{
	class SkinnedMesh;
}

namespace Ideal
{
	class IParticleSystem;
}

class Enemy;
class EnemyController;
class PlayerAnimator;


class RangerIdle
	: public AnimationState
{
private:


public:
	RangerIdle(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};


class RangerPursuit
	: public AnimationState
{
private:
	int lastFrame = 0;

public:
	RangerPursuit(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};


class RangerReturn
	: public AnimationState
{
private:


public:
	RangerReturn(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};


class RangerAttack
	: public AnimationState
{
private:
	int lastFrame = 0;

public:
	RangerAttack(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};


class RangerAim
	: public AnimationState
{
private:
	int lastFrame = 0;

public:
	RangerAim(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};


class RangerReload
	: public AnimationState
{
private:
	int lastFrame = 0;

public:
	RangerReload(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};


class RangerDamage
	: public AnimationState
{
private:


public:
	RangerDamage(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};


class RangerDown
	: public AnimationState
{
private:
	bool isReset = false;
	bool isChange = false;

public:
	RangerDown(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class RangerFall
	: public AnimationState
{
private:


public:
	RangerFall(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class RangerKnockBack
	: public AnimationState
{
private:


public:
	RangerKnockBack(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class RangerDeath
	: public AnimationState
{
private:


public:
	RangerDeath(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
};



// 애니메이터 없어서 임시로 만든 컴포넌트
class RangerAnimator :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(RangerAnimator);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	std::shared_ptr<Truth::SkinnedMesh> m_skinnedMesh;
	std::shared_ptr<Enemy> m_enemy;
	std::shared_ptr<EnemyController> m_enemyController;

	std::shared_ptr<PlayerAnimator> m_playerAnimator;

	std::shared_ptr<Ideal::IParticleSystem> m_magicCircle;
	std::shared_ptr<Ideal::IParticleSystem> m_shootEffect;


	/// <summary>
	///  상태 조절을 위한 것들
	/// </summary>
	/// <param name=""></param>
	/// 
	/// ----------------------------------------
	PROPERTY(isPursuit);
	bool m_isPursuit;

	PROPERTY(isReturn);
	bool m_isReturn;

	PROPERTY(isAttack);
	bool m_isAttack;

	PROPERTY(isDamage);
	bool m_isDamage;

	PROPERTY(isBack);
	bool m_isBack;

	PROPERTY(isFall);
	bool m_isFall;

	PROPERTY(isDown);
	bool m_isDown;

	PROPERTY(isDeath);
	bool m_isDeath;

	PROPERTY(isReload);
	bool m_isReload;

	PROPERTY(isShooting);
	bool m_isShooting;

	PROPERTY(showEffect);
	bool m_showEffect;
	
	/// ----------------------------------------

	/// 이펙트용

	PROPERTY(shootReady);
	bool m_shootReady;

	/// ----------------------------------------

	PROPERTY(passingTime);
	float m_passingTime;

	float m_lastHp;

	float m_baseSpeed;

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

	void PlayEffect();

public:
	RangerAnimator();
	virtual ~RangerAnimator();

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

	void SetImpulse(float power);

	void SoundPlay(std::wstring path, bool isDup, int channel);

};

template<class Archive>
void RangerAnimator::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(RangerAnimator)