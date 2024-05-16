#pragma once
#include "Headers.h"
#include "Entity.h"

class BulletShooter
	: public Truth::Entity
{
	GENERATE_CLASS_TYPE_INFO(BulletShooter);

public:
	BulletShooter();
	virtual ~BulletShooter();

	virtual void Initailize() override;
};

