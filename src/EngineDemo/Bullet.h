#pragma once
#include "Component.h"
#include "Collider.h"

class Bullet :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(Bullet);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	PROPERTY(bulletDamage);
	float m_bulletDamage;

	PROPERTY(isHit);
	bool m_isHit;


public:
	Vector3 m_power;

	Bullet();
	virtual ~Bullet();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

	METHOD(OnCollisionEnter);
	void OnCollisionEnter(Truth::Collider* _other);

};

template<class Archive>
void Bullet::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void Bullet::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(Bullet)
BOOST_CLASS_VERSION(Bullet, 0)