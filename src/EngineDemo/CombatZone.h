#pragma once
#include "Component.h"

class CombatZone :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(CombatZone);
private:
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	bool m_isTargetIn;
	std::shared_ptr<Truth::Entity> m_target;
	
public:
	CombatZone();
	virtual ~CombatZone();

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

private:
	std::shared_ptr<Truth::Entity> Detect();
};

template<class Archive>
void CombatZone::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void CombatZone::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(CombatZone)
BOOST_CLASS_VERSION(CombatZone, 0)