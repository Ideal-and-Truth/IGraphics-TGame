#include "InputManager.h"
#include "Managers.h"

Truth::InputManager::InputManager()
	: m_keyInfomation()
	, m_hwnd(nullptr)
	, m_currentMousePos{}
	, m_prevMousePos{}
{
	DEBUG_PRINT("Create InputManager\n");
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
				m_eventManager.lock()->PublishEvent(m_virtualKeyString[i] + "_" + m_keyStateString[(int)KEY_STATE::UP]);
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
				m_eventManager.lock()->PublishEvent(m_virtualKeyString[i] + "_" + m_keyStateString[(int)KEY_STATE::HOLD]);
			}
			else
			{
				m_eventManager.lock()->PublishEvent(m_virtualKeyString[i] + "_" + m_keyStateString[(int)KEY_STATE::DOWN]);
			}

			m_keyInfomation[i].prevPush = true;
		}
		else
		{
			if (m_keyInfomation[i].prevPush)
			{
				m_eventManager.lock()->PublishEvent(m_virtualKeyString[i] + "_" + m_keyStateString[(int)KEY_STATE::UP]);
			}

			m_keyInfomation[i].prevPush = false;
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

/// <summary>
/// ���콺�� �̵��� �Ÿ� 
/// </summary>
/// <returns>���콺�� �̵��� �Ÿ�</returns>
POINT Truth::InputManager::GetMouseMove() const
{
	POINT result = {};
	result.x = m_currentMousePos.x - m_prevMousePos.x;
	result.y = m_currentMousePos.y - m_prevMousePos.y;
	return result;
}

