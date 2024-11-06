#pragma once
#include "Component.h"
#include "Collider.h"

class Player;
class PlayerAnimator;
class Enemy;
class EnemyAnimator;
class BossAnimator;

class MeleeWeapon :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(MeleeWeapon);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	std::shared_ptr<Truth::Collider> m_collider;
	std::shared_ptr<Player> m_player;
	std::shared_ptr<PlayerAnimator> m_playerAnimator;

	std::shared_ptr<Enemy> m_enemy;
	std::shared_ptr<EnemyAnimator> m_enemyAnimator;
	std::shared_ptr<BossAnimator> m_bossAnimator;

	std::vector<std::shared_ptr<Truth::Entity>> m_onHitEnemys;

	bool m_isAttacking;
	bool m_canHit;
	
private:
	void PlayEffect(Vector3 pos);

public:
	MeleeWeapon();
	virtual ~MeleeWeapon();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

	METHOD(OnTriggerEnter);
	void OnTriggerEnter(Truth::Collider* _other);

	METHOD(OnTriggerExit);
	void OnTriggerExit(Truth::Collider* _other);
};

template<class Archive>
void MeleeWeapon::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(MeleeWeapon)