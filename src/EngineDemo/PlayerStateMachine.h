#pragma once
#include "AnimatorController.h"

class PlayerStateMachine :
	public Truth::AnimatorController
{
private:
	Truth::AnimatorState m_idle;
	Truth::AnimatorState m_run;
	Truth::AnimatorState m_attack;

	bool m_isRunning;

public:
	PlayerStateMachine();
	virtual ~PlayerStateMachine();

	METHOD(Initalize);
	void Initalize();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();
};

