#pragma once
#include "Headers.h"
#include "Entity.h"

class Bullet 
    : public Truth::Entity
{
	GENERATE_CLASS_TYPE_INFO(Bullet)

public:
	Bullet();
	virtual ~Bullet();
	virtual void Initailize() override;
};

