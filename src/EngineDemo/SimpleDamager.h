#pragma once
#include "Component.h"
#include "Collider.h"

class Player;

class SimpleDamager :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(SimpleDamager);
private:
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::shared_ptr<Player> m_player;

	PROPERTY(user);
	std::shared_ptr<Truth::Entity> m_user;

	bool m_isPlayerIn;

	PROPERTY(onlyHitOnce);
	bool m_onlyHitOnce;

	PROPERTY(damage);
	float m_damage;

public:
	SimpleDamager();
	virtual ~SimpleDamager();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

	METHOD(OnTriggerEnter);
	void OnTriggerEnter(Truth::Collider* _other);

	METHOD(OnTriggerExit);
	void OnTriggerExit(Truth::Collider* _other);

	METHOD(OnCollisionEnter);
	void OnCollisionEnter(Truth::Collider* _other);

	METHOD(OnCollisionExit);
	void OnCollisionExit(Truth::Collider* _other);
};

template<class Archive>
void SimpleDamager::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void SimpleDamager::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(SimpleDamager)
BOOST_CLASS_VERSION(SimpleDamager, 0)