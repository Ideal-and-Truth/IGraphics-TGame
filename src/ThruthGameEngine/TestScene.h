#pragma once
#include "Headers.h"
#include "Scene.h"

class TestScene :
    public Scene
{
public:
	TestScene();
	virtual ~TestScene();

	virtual void FrontUpdate(float4 _dt);
	virtual void BackUpdate(float4 _dt);

	virtual void Enter();
	virtual void Exit();
};

