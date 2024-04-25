#pragma once
#include "Headers.h"
#include "Scene.h"

class TestScene2 :
    public Scene
{
public:
	TestScene2(std::shared_ptr<Managers> _managers);
	virtual ~TestScene2();

	virtual void Enter();
	virtual void Awake();
	virtual void Exit();
};

