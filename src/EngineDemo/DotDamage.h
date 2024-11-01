#pragma once
#include "Component.h"
#include "Collider.h"


namespace Ideal
{
	class IParticleSystem;
}

class Player;

class DotDamage :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(DotDamage);
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
	std::shared_ptr<Ideal::IParticleSystem> m_particle;

	PROPERTY(playEffect);
	bool m_playEffect;

private:
	void PlayEffect();

public:
	DotDamage();
	virtual ~DotDamage();

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
void DotDamage::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void DotDamage::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(DotDamage)
BOOST_CLASS_VERSION(DotDamage, 0)