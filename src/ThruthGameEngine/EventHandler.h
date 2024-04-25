#pragma once
#include "EventManager.h"

class EventHandler
{
public:
	std::string m_name;

public:
	// EventSystem �� ����ϱ� ���� Callable ����
	template <typename T>
	ListenerInfo MakeListenerInfo(void (T::* func)(std::any))
	{
		// ��ȿ�� �˻�
		static_assert(std::is_base_of_v<EventHandler, T>, "T must be derived from EventHandler");
		// Callable ��ȯ (���� ������ ListenerInfo ����ü ����)
		return 
			ListenerInfo(this, [this, func](std::any handler)
			{ 
				(static_cast<T*>(this)->*func)(handler);
			}
		);
	}
};

