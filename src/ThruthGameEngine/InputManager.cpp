#include "InputManager.h"

InputManager::InputManager()
	: m_keyInfomation()
	, m_hwnd(nullptr)
	, m_nowMousePosX(0)
	, m_nowMousePosY(0)
	, m_mouseInfomation()
	, m_mouseClick()
	, m_oldMousePosX(0)
	, m_oldMousePosY(0)
	, m_mouseDX(0)
	, m_mouseDY(0)
{

}

InputManager::~InputManager()
{

}

void InputManager::OnMouseLeftUp(int _x, int _y)
{
	ResetMouse(_x, _y);
	this->m_mouseClick[(int)MOUSE::LEFT] = false;
}

void InputManager::OnMouseLeftDown(int _x, int _y)
{
	ResetMouse(_x, _y);
	this->m_mouseClick[(int)MOUSE::LEFT] = true;
}

void InputManager::OnMouseRightUp(int _x, int _y)
{
	ResetMouse(_x, _y);
	this->m_mouseClick[(int)MOUSE::RIGHT] = false;
}

void InputManager::OnMouseRightDown(int _x, int _y)
{
	ResetMouse(_x, _y);
	this->m_mouseClick[(int)MOUSE::RIGHT] = true;
}

void InputManager::OnMouseMove(int _btnState, int _x, int _y)
{
	this->m_oldMousePosX = this->m_nowMousePosX;
	this->m_oldMousePosY = this->m_nowMousePosY;

	this->m_nowMousePosX = _x;
	this->m_nowMousePosY = _y;

	this->m_mouseDX = this->m_nowMousePosX - this->m_oldMousePosX;
	this->m_mouseDY = this->m_nowMousePosY - this->m_oldMousePosY;
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
	// ���� ��Ŀ�� [X] ���¸� Ű ��ü �� ���� ó���ϱ� / ������ �־����� ����
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

		for (size_t i = 0; i < static_cast<size_t>(MOUSE::END); i++)
		{
			m_mouseInfomation[i].prevPush = false;

			if (m_mouseInfomation[i].state == KEY_STATE::DOWN || m_mouseInfomation[i].state == KEY_STATE::HOLD)
			{
				m_mouseInfomation[i].state = KEY_STATE::UP;
			}
			if (m_mouseInfomation[i].state == KEY_STATE::UP)
			{
				m_mouseInfomation[i].state = KEY_STATE::NONE;
			}
		}
		return;
	}

	// ���� ��Ŀ�� [O] �� Ű ���� ������Ʈ
	for (size_t i = 0; i < static_cast<size_t>(KEY::END); ++i)
	{
		// Ű�� �����ִ�
		if (GetAsyncKeyState(m_virtualKeyArray[i]) & 0x8000)
		{
			if (m_keyInfomation[i].prevPush)
			{
				// �������� ���� �־���.
				m_keyInfomation[i].state = KEY_STATE::HOLD;
			}
			else
			{
				// ������ �������� �ʾҴ�.
				m_keyInfomation[i].state = KEY_STATE::DOWN;
			}

			m_keyInfomation[i].prevPush = true;
		}

		// Ű�� �ȴ����ִ�
		else
		{
			if (m_keyInfomation[i].prevPush)
			{
				// ������ ���� �־���.
				m_keyInfomation[i].state = KEY_STATE::UP;
			}
			else
			{
				// �������� �������� �ʾҴ�.
				m_keyInfomation[i].state = KEY_STATE::NONE;
			}

			m_keyInfomation[i].prevPush = false;
		}
		for (size_t i = 0; i < static_cast<size_t>(MOUSE::END); i++)
		{
			if (this->m_mouseClick[i])
			{
				if (this->m_mouseInfomation[i].prevPush)
				{
					this->m_mouseInfomation[i].state = KEY_STATE::HOLD;
				}
				else
				{
					this->m_mouseInfomation[i].state = KEY_STATE::DOWN;
				}
				this->m_mouseInfomation[i].prevPush = true;
			}
			else
			{
				if (this->m_mouseInfomation[i].prevPush)
				{
					this->m_mouseInfomation[i].state = KEY_STATE::UP;
				}
				else
				{
					this->m_mouseInfomation[i].state = KEY_STATE::NONE;
				}
				this->m_mouseInfomation[i].prevPush = false;
			}
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

void InputManager::ResetMouse(int _x /*= 0*/, int _y /*= 0*/)
{
	this->m_nowMousePosX = _x;
	this->m_oldMousePosX = _x;

	this->m_nowMousePosY = _y;
	this->m_oldMousePosY = _y;

	this->m_mouseDX = 0;
	this->m_mouseDY = 0;
}
