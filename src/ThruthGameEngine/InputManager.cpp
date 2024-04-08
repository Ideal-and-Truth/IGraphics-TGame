#include "InputManager.h"
#include "Managers.h"

InputManager::InputManager()
	: m_keyInfomation()
	, m_hwnd(nullptr)
	, m_currentMousePos{}
	, m_prevMousePos{}
{
	m_t = std::make_shared<bool>(true);
	m_f = std::make_shared<bool>(false);
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
	// ���� ��Ŀ�� [X] ���¸� Ű ��ü �� ���� ó���ϱ� / ������ �־����� ����
	HWND isGetFocusedHwnd = GetFocus();

	std::weak_ptr<EventManager> eventManager = Managers::Get()->Event();

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
				eventManager.lock()->PublishEvent(m_virtualKeyString[i] + "_" + m_keyStateString[(int)KEY_STATE::HOLD], std::reinterpret_pointer_cast<void>(m_t), 0);
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

