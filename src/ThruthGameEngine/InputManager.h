#pragma once
#include "Headers.h"
#include "EventHandler.h"

/// <summary>
/// 키 상태 
/// </summary>
enum class KEY_STATE
{
	NONE, // 키가 눌리지 않은 상태
	DOWN, // 키가 눌려진 상태 
	HOLD, // 키가 눌린 상태
	UP,	// 키가 올라온 상태
	END,
};

/// <summary>
/// 입력을 지원하는 키 
/// </summary>
enum class KEY
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	Q, W, E, R, T, Y, U, I, O, P,
	A, S, D, F, G, H, J, K, L,
	Z, X, C, V, B, N, M,
	N_1,
	N_2,
	N_3,
	N_4,
	N_5,
	N_6,
	N_7,
	N_8,
	N_9,
	N_0,

	ALT,
	CTRL,
	LSHIFT,
	SPACE,
	ENTER,
	ESC,
	LBTN,
	RBTN,

	F_1,
	F_2,
	F_3,
	F_4,
	F_5,
	F_6,
	F_7,
	F_8,
	F_9,
	F_10,
	F_11,
	F_12,
	BACKSPACE,

	LMOUSE, RMOUSE, UPBUTTON, DOWNBUTTON,

	END,
};

/// <summary>
/// 키 정보
/// </summary>
struct tKeyInfo
{
	KEY_STATE state;
	bool prevPush;
};

/// <summary>
/// input mamaging class
/// </summary>
class InputManager
{
private:
	// 윈도우 포커스 여부를 확인 할 윈도우 핸들러
	HWND m_hwnd;

	tKeyInfo m_keyInfomation[(int)KEY::END];

	// 키 입력을 확인할 버추얼 키 값
	int m_virtualKeyArray[(int)KEY::END] =
	{
		VK_LEFT,	//LEFT,
		VK_RIGHT,	//RIGHT,
		VK_UP,		//UP,
		VK_DOWN,	//DOWN,
		'Q','W','E','R','T','Y','U','I','O','P',
		'A','S','D','F','G','H','J','K','L',
		'Z','X','C','V','B','N','M',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'0',

		VK_MENU,	// ALT
		VK_CONTROL,	// CTRL
		VK_LSHIFT,	// LSHIFT
		VK_SPACE,	// SPACE BAR
		VK_RETURN,	// ENTER
		VK_ESCAPE,	// ESC

		VK_LBUTTON,
		VK_RBUTTON,
		VK_F1,
		VK_F2,
		VK_F3,
		VK_F4,
		VK_F5,
		VK_F6,
		VK_F7,
		VK_F8,
		VK_F9,
		VK_F10,
		VK_F11,
		VK_F12,

		VK_LBUTTON, VK_RBUTTON, VK_XBUTTON2, VK_XBUTTON1,

		VK_BACK
	};

	// 키 상태를 string 형태로 바꾼 것
	const std::string m_keyStateString[(int)KEY_STATE::END] =
	{
		"NONE", "DOWN", "HOLD", "UP"
	};

	// 키 입력을 확인할 버추얼 키 값을 string 형태로 바꾼 것
	const std::string m_virtualKeyString[(int)KEY::END] =
	{
		"LEFT",
		"RIGHT",
		"UP",
		"DOWN",
		"Q","W","E","R","T","Y","U","I","O","P",
		"A","S","D","F","G","H","J","K","L",
		"Z","X","C","V","B","N","M",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"0",

		"MENU",	// ALT
		"CONTROL",	// CTRL
		"LSHIFT",	// LSHIFT
		"SPACE",	// SPACE BAR
		"RETURN",	// ENTER
		"ESCAPE",	// ESC

		"LBUTTON",
		"RBUTTON",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",

		"LBUTTON", "RBUTTON", "XBUTTON2", "XBUTTON1",

		"BACK"
	};

private:
	// 현재 프레임 마우스 위치
	POINT m_currentMousePos;
	// 과거 프레임 마우스 위치
	POINT m_prevMousePos;

	// 이벤트 발행을 위한 이벤트 매니저
	// 그때 그때 생성하는거 보다 미리 받아놓는다.
	std::weak_ptr<EventManager> m_eventManager;

public:
	InputManager();
	~InputManager();

public:
	// 초기화
	void Initalize(HWND _hwnd, std::shared_ptr<EventManager> _eventManager);

	// 업데이트
	void Update();
	// 리셋
	void Reset();

	void Finalize();

	KEY_STATE GetKeyState(KEY _eKey) const = delete;

	POINT GetMouseMove() const;
};

