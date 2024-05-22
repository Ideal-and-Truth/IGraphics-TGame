#pragma once
#include "Entity.h"
class TriggerWall 
    : public Truth::Entity
{
	GENERATE_CLASS_TYPE_INFO(TriggerWall)

public:
	TriggerWall();
	virtual ~TriggerWall();
	virtual void Initailize() override;
};

