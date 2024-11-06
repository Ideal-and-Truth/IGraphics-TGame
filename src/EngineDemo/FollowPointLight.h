#pragma once
#include "PointLight.h"

class FollowPointLight
	: public Truth::PointLight
{
	GENERATE_CLASS_TYPE_INFO(FollowPointLight);
private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

public:
	FollowPointLight();
	virtual ~FollowPointLight();

#ifdef EDITOR_MODE
	virtual void EditorSetValue();
#endif // EDITOR_MODE

private:
	METHOD(Update);
	virtual void Update() override;


};

template<class Archive>
void FollowPointLight::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Truth::PointLight>(*this);
}

template<class Archive>
void FollowPointLight::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Truth::PointLight>(*this);
}


BOOST_CLASS_EXPORT_KEY(FollowPointLight)
BOOST_CLASS_VERSION(FollowPointLight, 2)