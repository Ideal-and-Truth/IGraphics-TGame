#pragma once
#include "Component.h"
class Shooter :
    public Truth::Component
{
    GENERATE_CLASS_TYPE_INFO(Shooter);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

public:
    Shooter();
    virtual ~Shooter();

    METHOD(Awake);
    void Awake();

    METHOD(Update);
    void Update();
};

template<class Archive>
void Shooter::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(Shooter)
