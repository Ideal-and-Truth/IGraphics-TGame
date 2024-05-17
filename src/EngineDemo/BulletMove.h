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

public:
    BulletMove();
    virtual ~BulletMove();

    METHOD(Start);
    void Start();

    void Update(std::any _p);

    METHOD(OnCollisionEnter);
    void OnCollisionEnter(const Truth::Collider* _other);
};

