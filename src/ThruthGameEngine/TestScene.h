#pragma once
#include "Headers.h"
#include "Scene.h"
namespace Truth
{
	class TestScene :
		public Scene
	{
	public:
		TestScene(std::shared_ptr<Managers> _managers);
		virtual ~TestScene();

		virtual void Enter();
		virtual void Awake();
		virtual void Exit();
	};
}

