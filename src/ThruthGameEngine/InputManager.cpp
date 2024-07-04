#include "InputManager.h"
#include "Managers.h"

Truth::InputManager::InputManager()
	: m_keyInfomation()
	, m_hwnd(nullptr)
	, m_currentMousePos{}
	, m_prevMousePos{}
	, m_oldMousePosX(0)
	, m_oldMousePosY(0)
	, m_nowMousePosX(0)
	, m_nowMousePosY(0)
	, m_mouseDx(0)
	, m_mouseDy(0)
{
	DEBUG_PRINT("Create InputManager\n");
	m_mousePoint = new POINT;
}

Truth::InputManager::~InputManager()
{
	DEBUG_PRINT("Finalize InputManager\n");
}

/// <summary>
/// 초기화
/// </summary>
/// <param name="_hwnd">윈도우 포커스 확인을 위한 핸들러</param>
void Truth::InputManager::Initalize(HWND _hwnd, std::shared_ptr<EventManager> _eventManager)
{
	m_hwnd = _hwnd;

	for (UINT i = 0; i < static_cast<UINT>(KEY::END); i++)
	{
		m_keyInfomation[i] = tKeyInfo{ KEY_STATE::NONE, false };
	}
	m_eventManager = _eventManager;
}

/// <summary>
/// 업데이트 합수
/// 키의 상태에 따라 이벤트를 발행한다.
/// </summary>
void Truth::InputManager::Update()
{
	// 게임 포커스 [X] 상태면 키 전체 안 누름 처리하기 / 누르고 있었으면 떼기
	HWND isGetFocusedHwnd = GetFocus();

	// 현재 윈도우가 포커스 상태가 아닌경우
	if (isGetFocusedHwnd == nullptr)
	{
		// 키 값을 리셋한다
		for (size_t i = 0; i < static_cast<size_t>(KEY::END); i++)
		{
			m_keyInfomation[i].prevPush = false;
			// 허나 눌려있던 상태라면 UP 상태로 바꿔 주어야 한다.
			if (m_keyInfomation[i].state == KEY_STATE::DOWN || m_keyInfomation[i].state == KEY_STATE::HOLD)
			{
				m_keyInfomation[(int)i].state = KEY_STATE::UP;
			}
			else
			{
				m_keyInfomation[(int)i].state = KEY_STATE::NONE;
			}
		}
		// 업데이트 바로 종료
		return;
	}

	// 게임 포커스 [O] 때 키 상태 업데이트
	for (size_t i = 0; i < static_cast<size_t>(KEY::END); ++i)
	{
		// 현재 키가 눌려있는지와 과거 눌려있는지를 비교하여 이벤트를 발행한다.
		if (GetAsyncKeyState(m_virtualKeyArray[i]) & 0x8000)
		{
			if (m_keyInfomation[i].prevPush)
			{
				m_keyInfomation[(int)i].state = KEY_STATE::HOLD;
			}
			else
			{
				m_keyInfomation[(int)i].state = KEY_STATE::DOWN;
			}

			m_keyInfomation[i].prevPush = true;
		}
		else
		{
			if (m_keyInfomation[i].prevPush)
			{
				m_keyInfomation[(int)i].state = KEY_STATE::UP;
			}
			else
			{
				m_keyInfomation[(int)i].state = KEY_STATE::NONE;
			}

			m_keyInfomation[i].prevPush = false;
		}
	}
	GetCursorPos(m_mousePoint);

	OnMouseMove(0, m_mousePoint->x, m_mousePoint->y);

	if (m_fpsMode)
	{
		SetCursorPos(1920 / 2, 1080 / 2);

		if (GetKeyState(KEY::ESC) == KEY_STATE::DOWN)
		{
			m_fpsMode = false;
		}
	}
	else
	{
		if (GetKeyState(KEY::F_1) == KEY_STATE::DOWN)
		{
			m_fpsMode = true;
		}
	}
}

/// <summary>
/// 리셋
/// 모든 키의 과거 상태를 눌리지 않는 상태로 바꾸낟.
/// </summary>
void Truth::InputManager::Reset()
{
	for (size_t i = 0; i < static_cast<size_t>(KEY::END); i++)
	{
		this->m_keyInfomation[i].prevPush = false;
	}
}

void Truth::InputManager::Finalize()
{
	Reset();
}



void Truth::InputManager::OnMouseMove(int _btnState, int _x, int _y)
{
	if (m_fpsMode)
	{
		m_oldMousePosX = 1920 / 2;
		m_oldMousePosY = 1080 / 2;
	}
	else
	{
		m_oldMousePosX = m_nowMousePosX;
		m_oldMousePosY = m_nowMousePosY;
	}

	m_nowMousePosX = _x;
	m_nowMousePosY = _y;

	m_mouseDx = m_nowMousePosX - m_oldMousePosX;
	m_mouseDy = m_nowMousePosY - m_oldMousePosY;
}

void Truth::InputManager::ResetMouseMovement(int _x /*= 0*/, int _y /*= 0*/)
{
	m_mouseDx = 0;
	m_mouseDy = 0;

}

KEY_STATE Truth::InputManager::GetKeyState(KEY _eKey) const
{
	return m_keyInfomation[(int)_eKey].state;
}

int16 Truth::InputManager::GetMouseMoveX() const
{
	return m_mouseDx;
}

int16 Truth::InputManager::GetMouseMoveY() const
{
	return m_mouseDy;
}

