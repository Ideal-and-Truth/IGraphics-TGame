#pragma once
#include "Headers.h"

/// <summary>
/// input mamaging class
/// </summary>

enum class KEY_STATE
{
	NONE, // Ű�� ������ ���� ����
	DOWN, // Ű�� ������ ���� 
	HOLD, // Ű�� ���� ����
	UP,	// Ű�� �ö�� ����
	END,
};

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

struct tKeyInfo
{
	KEY_STATE state;
	bool prevPush;
};

class InputManager
{
	friend class Processor;
private:
	HWND m_hwnd;

	tKeyInfo m_keyInfomation[(int)KEY::END];

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

private:
	POINT m_currentMousePos;
	POINT m_prevMousePos;

public:
	InputManager();
	~InputManager();

public:
	void Initalize(HWND _hwnd);

	void Update();
	void Reset();

	KEY_STATE GetKeyState(KEY _eKey) const { return m_keyInfomation[(int)_eKey].state; }

	POINT GetMouseMove();
};

