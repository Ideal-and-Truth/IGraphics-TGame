#pragma once
#include "Component.h"

class TestMovement
	: public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(TestMovement);

public:
	TestMovement();
	virtual ~TestMovement();

	METHOD(Update);
	void Update();
};

