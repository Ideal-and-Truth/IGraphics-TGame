#pragma once
#include "Component.h"
class HitStop :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(HitStop);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:


public:
	HitStop();
	virtual ~HitStop();

	METHOD(Awake);
	void Awake();

	METHOD(Update);
	void Update();
};

template<class Archive>
void HitStop::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);

}

template<class Archive>
void HitStop::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);

}


BOOST_CLASS_EXPORT_KEY(HitStop)
BOOST_CLASS_VERSION(HitStop, 0)