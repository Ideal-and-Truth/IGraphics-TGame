#pragma once
#include "Headers.h"

class TestScene;
class Managers;

/// <summary>
/// ������ ���μ���
/// ���� ��ü�� ������ ����Ѵ�.
/// </summary>
class Processor
{
private:
	std::shared_ptr<Managers> m_manager;

	// ������ ������ ���� ����
	HWND m_hwnd;
	MSG m_msg;

public:
	// Ư�� ����Լ�
	Processor();
	~Processor();

	// �ʱ�ȭ
	void Initialize(HINSTANCE _hInstance);
	void Finalize();
	void Process();
	void Loop();

	// ������ �Լ�
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	void CreateMainWindow(HINSTANCE _hInstance, int _width = 1920, int _height = 1080, const wchar_t szAppName[] = L"Truth Engine");
	void InitializeManager();
};

