#include "InputManager.h"

InputManager::InputManager()
	: m_keyInfomation()
	, m_hwnd(nullptr)
	, m_currentMousePos{}
	, m_prevMousePos{}
{

}

InputManager::~InputManager()
{

}


void InputManager::Initalize(HWND _hwnd)
{
	m_hwnd = _hwnd;

	for (UINT i = 0; i < static_cast<UINT>(KEY::END); i++)
	{
		m_keyInfomation[i] = tKeyInfo{ KEY_STATE::NONE, false };
	}
}


void InputManager::Update()
{
	// 게임 포커스 [X] 상태면 키 전체 안 누름 처리하기 / 누르고 있었으면 떼기
	HWND isGetFocusedHwnd = GetFocus();

	if (isGetFocusedHwnd == nullptr)
	{
		for (size_t i = 0; i < static_cast<size_t>(KEY::END); i++)
		{
			m_keyInfomation[i].prevPush = false;

			if (m_keyInfomation[i].state == KEY_STATE::DOWN || m_keyInfomation[i].state == KEY_STATE::HOLD)
			{
				m_keyInfomation[i].state = KEY_STATE::UP;
			}
			if (m_keyInfomation[i].state == KEY_STATE::UP)
			{
				m_keyInfomation[i].state = KEY_STATE::NONE;
			}
		}
		return;
	}

	// 게임 포커스 [O] 때 키 상태 업데이트
	for (size_t i = 0; i < static_cast<size_t>(KEY::END); ++i)
	{
		// 키가 눌려있다
		if (GetAsyncKeyState(m_virtualKeyArray[i]) & 0x8000)
		{
			if (m_keyInfomation[i].prevPush)
			{
				// 이전에도 눌려 있었다.
				m_keyInfomation[i].state = KEY_STATE::HOLD;
			}
			else
			{
				// 이전에 눌려있지 않았다.
				m_keyInfomation[i].state = KEY_STATE::DOWN;
			}

			m_keyInfomation[i].prevPush = true;
		}

		// 키가 안눌려있다
		else
		{
			if (m_keyInfomation[i].prevPush)
			{
				// 이전에 눌려 있었다.
				m_keyInfomation[i].state = KEY_STATE::UP;
			}
			else
			{
				// 이전에도 눌려있지 않았다.
				m_keyInfomation[i].state = KEY_STATE::NONE;
			}

			m_keyInfomation[i].prevPush = false;
		}
	}
}

void InputManager::Reset()
{
	for (size_t i = 0; i < static_cast<size_t>(KEY::END); i++)
	{
		this->m_keyInfomation[i].state = KEY_STATE::NONE;
		this->m_keyInfomation[i].prevPush = false;
	}
}

POINT InputManager::GetMouseMove() const
{
	POINT result = {};
	result.x = m_currentMousePos.x - m_prevMousePos.x;
	result.y = m_currentMousePos.y - m_prevMousePos.y;
	return result;
}

