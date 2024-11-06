#pragma once
#include "Component.h"
#include "AnimationState.h"
#include "Collider.h"


namespace Truth
{
	class SkinnedMesh;
}

namespace Ideal
{
	class IParticleSystem;
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
	int lastFrame = 0;

public:
	PlayerRun(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
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

class NormalAbility
	: public AnimationState
{
private:
	bool isReset = false;

public:
	NormalAbility(Truth::Component* animator)
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

class ChargedAbility
	: public AnimationState
{
private:
	bool isReset = false;

public:
	ChargedAbility(Truth::Component* animator)
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
	bool isReset = false;

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

class PlayerRushAttack
	: public AnimationState
{
private:

public:
	PlayerRushAttack(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class PlayerSkillQ
	: public AnimationState
{
private:

public:
	PlayerSkillQ(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class PlayerSkillE
	: public AnimationState
{
private:
	bool isReset = false;
	int lastFrame = 0;
public:
	PlayerSkillE(Truth::Component* animator)
		: AnimationState(animator)
	{

	}

public:
	virtual void OnStateEnter() override;
	virtual void OnStateUpdate() override;
	virtual void OnStateExit() override;
};

class PlayerParry
	: public AnimationState
{
private:
	bool isReset = false;
	bool isChange = false;

public:
	PlayerParry(Truth::Component* animator)
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
	std::shared_ptr<PlayerCamera> m_playerCamera;
	std::shared_ptr<Ideal::IParticleSystem> m_normalAbilityEffect;


	/// <summary>
	///  상태 조절을 위한 것들
	/// </summary>
	/// <param name=""></param>
	/// 
	/// ----------------------------------------
	PROPERTY(isRun);
	bool m_isRun;

	PROPERTY(isAttack);
	bool m_isAttack;

	PROPERTY(skillQ);
	bool m_skillQ;

	PROPERTY(skillE);
	bool m_skillE;

	PROPERTY(coolTimeE);
	float m_coolTimeE;

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

	PROPERTY(backAttack);
	bool m_backAttack;

	PROPERTY(fallAttack);
	bool m_fallAttack;

	PROPERTY(downAttack);
	bool m_downAttack;

	PROPERTY(chargedReady);
	bool m_chargedReady;

	PROPERTY(parry);
	bool m_parry;

	PROPERTY(rush);
	bool m_rush;

	PROPERTY(forwardInput);
	float m_forwardInput;

	PROPERTY(sideInput);
	float m_sideInput;

	/// ----------------------------------------

	// 이펙트때문에 임시로
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

	PROPERTY(normalAbility);
	bool m_normalAbility;

	PROPERTY(chargedAttack1);
	bool m_chargedAttack1;

	PROPERTY(chargedAttack2);
	bool m_chargedAttack2;

	PROPERTY(chargedAttack3);
	bool m_chargedAttack3;

	PROPERTY(chargedAttack4);
	bool m_chargedAttack4;

	PROPERTY(chargedAttack5);
	bool m_chargedAttack5;

	PROPERTY(chargedAbility);
	bool m_chargedAbility;

	PROPERTY(dodgeAttack);
	bool m_dodgeAttack;

	PROPERTY(rushAttack);
	bool m_rushAttack;

	PROPERTY(swordBeam);
	bool m_swordBeam;

	PROPERTY(timeStop);
	bool m_timeStop;

	PROPERTY(hit);
	bool m_hit;

	float m_lastHp;

	PROPERTY(passingTime);
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

private:
	void PlayEffects();
	void HitSounds();

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

	void CameraShake(float shakeCount);

	void CameraZoom(float timing);

	void SetTimeSlow();

	void SoundPlay(std::wstring path, bool isDup, int channel);

	void SoundStop(int channel);

	inline bool GetBackAttack() const { return m_backAttack; }
	inline bool GetFallAttack() const { return m_fallAttack; }
	inline bool GetDownAttack() const { return m_downAttack; }
};

template<class Archive>
void PlayerAnimator::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerAnimator)