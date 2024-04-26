#pragma once
#include "Headers.h"

class TestScene;
class Managers;
class IdealRenderer;

/// <summary>
/// ������ ���μ���
/// ���� ��ü�� ������ ����Ѵ�.
/// </summary>
class Processor
{
private:
	std::shared_ptr<Managers> m_manager;
	std::shared_ptr<Ideal::IdealRenderer> m_renderer;
	std::shared_ptr<Ideal::IMeshObject> mesh;
	// ������ ������ ���� ����
	HWND m_hwnd;
	MSG m_msg;

	uint32 m_wight;
	uint32 m_height;


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

	void CreateMainWindow(HINSTANCE _hInstance, uint32 _width = 1920, uint32 _height = 1080, const wchar_t szAppName[] = L"Truth Engine");
	void InitializeManager();
	void CreateRender();
};

