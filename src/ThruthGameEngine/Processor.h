#pragma once
#include "Headers.h"
#include "Managers.h"
#include "SceneManager.h"

namespace Truth
{
	class Managers;
	class Scene;
}
	class EditorUI;

namespace Ideal
{
	class IdealRenderer;
	class IRenderScene;
	class ISkinnedMeshObject;
}

/// <summary>
/// 엔진의 프로세서
/// 엔진 전체의 진행을 담당한다.
/// </summary>
class Processor
{
private:
	EditorUI* m_editor;
	std::shared_ptr<Truth::Managers> m_manager;

	std::shared_ptr<Ideal::IMeshObject> mesh;
	std::shared_ptr<Ideal::ISkinnedMeshObject> m_cat;
	std::shared_ptr<Ideal::IAnimation> m_walkAnim;
	// 윈도우 생성을 위한 변수
	HWND m_hwnd;
	MSG m_msg;

	static Ideal::IdealRenderer* g_Renderer;
	static Truth::InputManager* g_inputmanager;

	uint32 m_wight;
	uint32 m_height;

	bool show_demo_window = true;

	const std::string m_savedFilePath = "../Scene/";

public:
	// 특수 멤버함수
	Processor();
	~Processor();

	// 초기화
	void Initialize(HINSTANCE _hInstance);
	void Finalize();
	void Process();
	void Loop();

	void AddScene(std::shared_ptr<Truth::Scene> _scene);

	void SaveCurrentScene();
	void SaveScene(std::shared_ptr<Truth::Scene> _scene);
	void LoadScene(std::string _path);

	void SetStartScene(std::string _name);

	// 윈도우 함수
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	// LRESULT CALLBACK WndProcInClass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	std::shared_ptr<Truth::Managers> GetManagers() const { return m_manager; };

private:
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	void CreateMainWindow(HINSTANCE _hInstance, uint32 _width = 1920, uint32 _height = 1080, const wchar_t szAppName[] = L"Truth Engine");
	void InitializeManager();
};



