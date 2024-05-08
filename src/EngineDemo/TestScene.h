#pragma once
#include "Headers.h"
#include "Scene.h"

class TestScene :
	public Truth::Scene
{
public:
	TestScene(std::shared_ptr<Truth::Managers> _managers);
	virtual ~TestScene();

	virtual void Enter();
	virtual void Awake();
	virtual void Exit();
};


