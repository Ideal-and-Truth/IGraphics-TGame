#pragma once
#include "Component.h"
class Shooter :
    public Truth::Component
{
    GENERATE_CLASS_TYPE_INFO(Shooter)

public:
    Shooter();
    virtual ~Shooter();

    METHOD(Awake);
    void Awake();

    METHOD(Update);
    void Update();
};

