#pragma once
#include "Headers.h"

class EventHandler;

/// 이벤트 실행을 위해 사용되는 구조체
struct Event
{
	std::string m_eventID;
	std::any m_parameter;
	float m_delayedTime;

	Event(std::string _id, std::any _param, const float& _delayed)
		: m_eventID(_id), m_parameter(_param), m_delayedTime(_delayed) {}
};

/// <summary>
/// 이벤트 리스너 구조체
/// 리스너 클래스 : 이벤트가 발생 할 때 영향을 받을 리스너 클래스
/// 이벤트 함수 : 이벤트 발생 시 실행할 함수. 리턴값은 항상 void
/// 이벤트 함수는 private 여도 상관 없다
/// </summary>
struct ListenerInfo
{
	const EventHandler* m_listener = nullptr;
	std::function<void(std::any)> EventFunction;

	ListenerInfo(const EventHandler* _listener, const std::function<void(std::any)>& _func)
		: m_listener(_listener), EventFunction(_func) {}
};

/// <summary>
/// 이벤트를 관리하는 매니저
/// 동적으로 이벤트를 생성하고 붙인다.
/// 
/// 이벤트 : 실행 함수
/// 이벤트ID : 해당 이벤트의 이름
/// 리스너 : 해당 이벤트를 구독한 클래스 (컴포넌트)
/// 구독 : 해당 이벤트가 발생할 때 특정 행동을 함
/// </summary>

class TimeManager;

class EventManager
{
private:
	// 이벤트의 빠른 검색을 위한 자료구조
	struct EventSearch
	{
		std::list<ListenerInfo>& m_listRef;
		std::list<ListenerInfo>::iterator m_itr;
		const std::string m_eventID;
	};

	std::queue<Event> m_customEvents;

	// 이벤트와 그 이벤트를 구독한 리스너
	std::unordered_map<std::string, std::list<ListenerInfo>> m_listeners;

	// 특정 리스너의 제거를 위해 어떤 리스너가 어디에 위치해 있는지 기록
	std::unordered_map<const EventHandler*, std::list<EventSearch>> m_eventHandlerInfo;

	std::weak_ptr<TimeManager> m_timeManager;

public:
	EventManager();
	~EventManager();

public:
	// 생성자
	void Initialize(std::weak_ptr<TimeManager> _timeManager);
	// 업데이트
	void Update();
	// 소멸
	void Finalize();

public:
	// 이벤트 발행 (_eventID에 해당하는 이벤트가 발생했다고 알림)
	void PublishEvent(std::string _eventID, const std::any _param = nullptr, const float& _delayed = 0.0f);
	// 이벤트 구독(특정 이벤트가 발생하는 것을 보고있음)
	void Subscribe(std::string _eventID, const ListenerInfo& _listenerInfo);

	// 구독 취소
	void Unsubscribe(std::string _eventID, const EventHandler* _listener);
	// 모든 이벤트에서 특정 리스너 제거
	void RemoveListener(const EventHandler* _listener);
	// 특정 이벤트에서 모든 리스너 제거
	void RemoveListenersAtEvent(std::string _eventID);
	// 모든 이벤트 제거
	void RemoveAllEvents();
	// 모든 구독 클래스 제거
	void RemoveAllSubscribes();

private:
	// 이벤트 실행
	void ProcessEvent();
	// 이벤트 함수 실행
	void DispatchEvent(const Event& _event);
	// 구독 여부 확인
	bool CheckSubscribe(std::string _eventID, const EventHandler* _listener);
};

