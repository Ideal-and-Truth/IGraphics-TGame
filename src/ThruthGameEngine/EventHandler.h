#pragma once
#include "EventManager.h"
namespace Truth
{
	class EventHandler
	{
	public:
		std::string m_name;

	public:
		// EventSystem 에 등록하기 위한 Callable 생성
		template <typename T>
		ListenerInfo MakeListenerInfo(void (T::* func)(std::any))
		{
			// 유효성 검사
			static_assert(std::is_base_of_v<EventHandler, T>, "T must be derived from EventHandler");
			// Callable 반환 (인자 설명은 ListenerInfo 구조체 참조)
			return
				ListenerInfo(this, [this, func](std::any handler)
					{
						(static_cast<T*>(this)->*func)(handler);
					}
			);
		}
	};
}

