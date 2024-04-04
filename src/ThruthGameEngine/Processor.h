#pragma once
#include "Headers.h"
#include "InputManager.h"

class TimeManager;
class InputManager;
class TestScene;

/// <summary>
/// 엔진의 프로세서
/// 엔진 전체의 진행을 담당한다.
/// </summary>
class Processor
{
private:
	// 매니저들
	std::unique_ptr<TimeManager> m_timeManager;
	std::unique_ptr<InputManager> m_inputManager;

	// 윈도우 생성을 위한 변수
	HWND m_hwnd;
	MSG m_msg;


/// 디버깅용 변수
	std::unique_ptr<TestScene> m_ts;

public:
	// 특수 멤버함수
	Processor();
	~Processor();

	// 초기화
	void Initialize(HINSTANCE _hInstance);
	void Process();
	void Loop();

	// 윈도우 함수
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	void CreateMainWindow(HINSTANCE _hInstance, int _width = 1920, int _height = 1080);
	void InitializeManager();
};

