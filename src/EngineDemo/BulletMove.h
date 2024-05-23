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

    METHOD(Update);
    void Update();

    METHOD(OnCollisionEnter);
    void OnCollisionEnter(Truth::Collider* _other);
};

