#pragma once
#include "EventManager.h"

class EventHandler
{
public:
	// EventSystem �� ����ϱ� ���� Callable ����
	template <typename T>
	ListenerInfo MakeListenerInfo(void (T::* func)(std::shared_ptr<void>))
	{
		// ��ȿ�� �˻�
		static_assert(std::is_base_of_v<EventHandler, T>, "T must be derived from EventHandler");
		// Callable ��ȯ (���� ������ ListenerInfo ����ü ����)
		return 
			ListenerInfo(this, [this, func](std::shared_ptr<void> handler) 
			{ 
				(reinterpret_cast<T*>(this)->*func)(handler);
			}
		);
	}
};

