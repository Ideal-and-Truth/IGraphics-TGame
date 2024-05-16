#pragma once
#include "Component.h"
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
};

