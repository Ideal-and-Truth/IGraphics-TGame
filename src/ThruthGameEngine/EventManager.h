#pragma once
#include "Headers.h"

class EventHandler;

/// �̺�Ʈ ������ ���� ���Ǵ� ����ü
struct Event
{
	std::string m_eventID;
	std::shared_ptr<void> m_parameter;
	float m_delayedTime;

	Event(std::string _id, std::shared_ptr<void> _param, const float& _delayed)
		: m_eventID(_id), m_parameter(_param), m_delayedTime(_delayed) {}
};

/// <summary>
/// �̺�Ʈ ������ ����ü
/// </summary>
struct ListenerInfo
{
	const EventHandler* m_listener = nullptr;
	std::function<void(std::shared_ptr<void>)> EventFunction;

	ListenerInfo(const EventHandler* _listener, const std::function<void(std::shared_ptr<void>)>& _func)
		: m_listener(_listener), EventFunction(_func) {}
};

/// <summary>
/// �̺�Ʈ�� �����ϴ� �Ŵ���
/// �������� �̺�Ʈ�� �����ϰ� ���δ�.
/// </summary>
class EventManager
{
private:
	std::queue<Event> m_customEvents;
	std::multimap<std::string, ListenerInfo> m_listeners;

public:
	EventManager();
	~EventManager();

public:
	void Initialize();
	void Update(const float4& _deltaTime);
	void Finalize();

public:
	void PublishEvent(std::string _eventID, const std::shared_ptr<void>& _param = nullptr, const float& _delayed = 0.0f);
	void Subscribe(std::string _eventID, const ListenerInfo& _listenerInfo);

	void Unsubscribe(std::string _eventID, const EventHandler* _listener);
	void RemoveListener(const EventHandler* _listener);
	void RemoveListenersAtEvent(std::string _eventID);
	void RemoveAllEvents();
	void RemoveAllSubscribes();

private:
	void ProcessEvent(const float4& _deltaTime);
	void DispatchEvent(const Event& _event);
	bool CheckSubscribe(std::string _eventID, const EventHandler* _listener);
};

