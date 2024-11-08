#pragma once
#include "Component.h"

class MapMoveTrigger :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(MapMoveTrigger);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

	PROPERTY(nextMap);
	std::string m_nextMap;

private:
	void PortalEffect();

public:
	MapMoveTrigger();
	virtual ~MapMoveTrigger();

	METHOD(OnTriggerEnter);
	virtual void OnTriggerEnter(Truth::Collider* _other) override;

	METHOD(Start);
	virtual void Start() override;

	METHOD(Update);
	virtual void Update() override;
};

template<class Archive>
void MapMoveTrigger::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);
	_ar& m_nextMap;
}

template<class Archive>
void MapMoveTrigger::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
	_ar& m_nextMap;
}


BOOST_CLASS_EXPORT_KEY(MapMoveTrigger)
BOOST_CLASS_VERSION(MapMoveTrigger, 0)
