#pragma once
#include "Headers.h"
#include "Entity.h"

class Wall
	: public Truth::Entity
{
	GENERATE_CLASS_TYPE_INFO(Wall)

public:
	Wall();
	virtual ~Wall();
	virtual void Initailize() override;

};

