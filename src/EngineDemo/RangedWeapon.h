#pragma once
#include "Component.h"
#include "Collider.h"

class Player;
class PlayerAnimator;
class Enemy;
class RangerAnimator;

class RangedWeapon :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(RangedWeapon);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::vector<std::pair<std::shared_ptr<Truth::Entity>, float>> m_bullets;

	std::shared_ptr<Truth::Collider> m_collider;

	std::shared_ptr<Player> m_player;
	std::shared_ptr<PlayerAnimator> m_playerAnimator;

	std::shared_ptr<Enemy> m_enemy;
	std::shared_ptr<RangerAnimator> m_rangerAnimator;


	bool m_isAttacking;
	bool m_isShot;
public:
	RangedWeapon();
	virtual ~RangedWeapon();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

};

template<class Archive>
void RangedWeapon::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void RangedWeapon::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(RangedWeapon)
BOOST_CLASS_VERSION(RangedWeapon, 0)