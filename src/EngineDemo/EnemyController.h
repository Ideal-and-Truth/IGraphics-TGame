#pragma once
#include "Component.h"

namespace Truth
{
	class Controller;
}

class Enemy;
class BossAnimator;

class EnemyController :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(EnemyController);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::weak_ptr<Truth::Controller> m_controller;
	std::shared_ptr<BossAnimator> m_bossAnimator;
	std::weak_ptr<Enemy> m_enemy;
	std::weak_ptr<Truth::Entity> m_target;

	Vector3 m_homePos;
	float m_speed;
	float m_passingTime;

	PROPERTY(impulsePower);
	float m_impulsePower;

	PROPERTY(sideMove);
	float m_sideMove;

	PROPERTY(isBackStep);
	bool m_isBackStep;

	PROPERTY(isComeBack);
	bool m_isComeBack;

	PROPERTY(isAttackReady);
	bool m_isAttackReady;

	PROPERTY(isDead);
	bool m_isDead;

	PROPERTY(canMove);
	bool m_canMove;

	PROPERTY(strafeMove);
	bool m_strafeMove;

	PROPERTY(attackCharge);
	bool m_attackCharge;

	PROPERTY(useImpulse);
	bool m_useImpulse;

public:
	EnemyController();
	virtual ~EnemyController();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

private:
	void FollowTarget();
	void ComeBackHome();
};

template<class Archive>
void EnemyController::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void EnemyController::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(EnemyController)
BOOST_CLASS_VERSION(EnemyController, 0)