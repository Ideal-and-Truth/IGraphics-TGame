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
/// �ʱ�ȭ
/// </summary>
/// <param name="_hwnd">������ ��Ŀ�� Ȯ���� ���� �ڵ鷯</param>
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
/// ������Ʈ �ռ�
/// Ű�� ���¿� ���� �̺�Ʈ�� �����Ѵ�.
/// </summary>
void Truth::InputManager::Update()
{
	// ���� ��Ŀ�� [X] ���¸� Ű ��ü �� ���� ó���ϱ� / ������ �־����� ����
	HWND isGetFocusedHwnd = GetFocus();

	// ���� �����찡 ��Ŀ�� ���°� �ƴѰ��
	if (isGetFocusedHwnd == nullptr)
	{
		// Ű ���� �����Ѵ�
		for (size_t i = 0; i < static_cast<size_t>(KEY::END); i++)
		{
			m_keyInfomation[i].prevPush = false;
			// �㳪 �����ִ� ���¶�� UP ���·� �ٲ� �־�� �Ѵ�.
			if (m_keyInfomation[i].state == KEY_STATE::DOWN || m_keyInfomation[i].state == KEY_STATE::HOLD)
			{
				m_keyInfomation[(int)i].state = KEY_STATE::UP;
			}
			else
			{
				m_keyInfomation[(int)i].state = KEY_STATE::NONE;
			}
		}
		// ������Ʈ �ٷ� ����
		return;
	}

	// ���� ��Ŀ�� [O] �� Ű ���� ������Ʈ
	for (size_t i = 0; i < static_cast<size_t>(KEY::END); ++i)
	{
		// ���� Ű�� �����ִ����� ���� �����ִ����� ���Ͽ� �̺�Ʈ�� �����Ѵ�.
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
/// ����
/// ��� Ű�� ���� ���¸� ������ �ʴ� ���·� �ٲٳ�.
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

