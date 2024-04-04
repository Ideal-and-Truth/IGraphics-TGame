#pragma once
#include "Headers.h"
#include "InputManager.h"

class TimeManager;
class InputManager;

class Processor
{
private:
	std::unique_ptr<TimeManager> m_timeManager;
	std::unique_ptr<InputManager> m_inputManager;

	HWND m_hwnd;
	MSG m_msg;

public:
	Processor();
	~Processor();

	void Initialize(HINSTANCE _hInstance);
	void Process();
	void Loop();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();
};

