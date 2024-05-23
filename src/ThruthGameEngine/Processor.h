#pragma once
#include "Headers.h"
#include "Managers.h"
#include "SceneManager.h"

namespace Truth
{
	class Managers;
	class Scene;
}

namespace Ideal
{
	class IdealRenderer;
	class IRenderScene;
	class ISkinnedMeshObject;
}

/// <summary>
/// ������ ���μ���
/// ���� ��ü�� ������ ����Ѵ�.
/// </summary>
class Processor
{
private:
	std::shared_ptr<Truth::Managers> m_manager;

	std::shared_ptr<Ideal::IMeshObject> mesh;
	std::shared_ptr<Ideal::ISkinnedMeshObject> m_cat;
	std::shared_ptr<Ideal::IAnimation> m_walkAnim;
	// ������ ������ ���� ����
	HWND m_hwnd;
	MSG m_msg;

	static Ideal::IdealRenderer* g_Renderer;

	uint32 m_wight;
	uint32 m_height;

	bool show_demo_window = true;

public:
	// Ư�� ����Լ�
	Processor();
	~Processor();

	// �ʱ�ȭ
	void Initialize(HINSTANCE _hInstance);
	void Finalize();
	void Process();
	void Loop();

	template<typename S, typename std::enable_if_t<std::is_base_of_v<Truth::Scene, S>, S>* = nullptr>
	void AddScene(std::string _name);

	void SetStartScene(std::string _name);

	// ������ �Լ�
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	// LRESULT CALLBACK WndProcInClass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	void CreateMainWindow(HINSTANCE _hInstance, uint32 _width = 1920, uint32 _height = 1080, const wchar_t szAppName[] = L"Truth Engine");
	void InitializeManager();
};

template<typename S, typename std::enable_if_t<std::is_base_of_v<Truth::Scene, S>, S>*>
void Processor::AddScene(std::string _name)
{
	m_manager->Scene()->AddScene<S>(_name, m_manager);
}

