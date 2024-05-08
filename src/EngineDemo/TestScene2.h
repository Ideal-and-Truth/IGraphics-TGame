#pragma once
#include "Headers.h"
#include "Scene.h"

	class TestScene2 :
		public Truth::Scene
	{
	public:
		TestScene2(std::shared_ptr<Truth::Managers> _managers);
		virtual ~TestScene2();

		virtual void Enter();
		virtual void Awake();
		virtual void Exit();
	};


