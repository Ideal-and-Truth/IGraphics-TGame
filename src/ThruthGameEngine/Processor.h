#pragma once
#include "Headers.h"
#include "InputManager.h"

class TimeManager;
class InputManager;
class TestScene;

/// <summary>
/// ������ ���μ���
/// ���� ��ü�� ������ ����Ѵ�.
/// </summary>
class Processor
{
private:
	// �Ŵ�����
	std::unique_ptr<TimeManager> m_timeManager;
	std::unique_ptr<InputManager> m_inputManager;

	// ������ ������ ���� ����
	HWND m_hwnd;
	MSG m_msg;


/// ������ ����
	std::unique_ptr<TestScene> m_ts;

public:
	// Ư�� ����Լ�
	Processor();
	~Processor();

	// �ʱ�ȭ
	void Initialize(HINSTANCE _hInstance);
	void Process();
	void Loop();

	// ������ �Լ�
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	void CreateMainWindow(HINSTANCE _hInstance, int _width = 1920, int _height = 1080);
	void InitializeManager();
};

