#pragma once
#include "Component.h"

namespace Truth
{
    class Collider;
}

class BulletMove :
    public Truth::Component
{
    GENERATE_CLASS_TYPE_INFO(BulletMove);

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

public:
    BulletMove();
    virtual ~BulletMove();

    METHOD(Start);
    void Start();

    METHOD(Update);
    void Update();

    METHOD(OnCollisionEnter);
    void OnCollisionEnter(Truth::Collider* _other);
};

template<class Archive>
void BulletMove::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}
