#include "EventManager.h"

EventManager::EventManager()
{

}

EventManager::~EventManager()
{

}

void EventManager::Initialize()
{

}

void EventManager::Update(const float4& _deltaTime)
{
	ProcessEvent(_deltaTime);
}

void EventManager::Finalize()
{
	RemoveAllEvents();
	RemoveAllSubscribes();
}

void EventManager::PublishEvent(std::string _eventID, const std::shared_ptr<void>& _param /*= nullptr*/, const float& _delayed /*= 0.0f*/)
{
	m_customEvents.push(Event{ _eventID, _param, _delayed });
}

void EventManager::Subscribe(std::string _eventID, const ListenerInfo& _listenerInfo)
{
	if (_listenerInfo.m_listener == nullptr ||
		CheckSubscribe(_eventID, _listenerInfo.m_listener))
	{
		return;
	}
	m_listeners.emplace(std::make_pair(_eventID, _listenerInfo));
}

void EventManager::Unsubscribe(std::string _eventID, const EventHandler* _listener)
{
	auto eRange = m_listeners.equal_range(_eventID);
	for (auto& itr = eRange.first; itr != eRange.second; )
	{
		if (itr->second.m_listener == _listener)
		{
			itr = m_listeners.erase(itr);
			continue;
		}
		++itr;
	}
}

// O(n) 개선이 필요함
void EventManager::RemoveListener(const EventHandler* _listener)
{
	for (auto itr = m_listeners.begin(); itr != m_listeners.end(); )
	{
		if (itr->second.m_listener == _listener)
		{
			itr = m_listeners.erase(itr);
			continue;
		}
		++itr;
	}
}

void EventManager::RemoveListenersAtEvent(std::string _eventID)
{
	auto eRange = m_listeners.equal_range(_eventID);

	for (auto& itr = eRange.first; itr != eRange.second; )
	{
		itr = m_listeners.erase(itr);
	}
}

void EventManager::RemoveAllEvents()
{
	while (!m_customEvents.empty())
	{
		m_customEvents.pop();
	}
}

void EventManager::RemoveAllSubscribes()
{
	m_listeners.clear();
}

void EventManager::ProcessEvent(const float4& _deltaTime)
{
	for (int i = 0; i < m_customEvents.size(); i++)
	{
		Event& e = m_customEvents.front();

		e.m_delayedTime -= _deltaTime;
		if (e.m_delayedTime <= 0.0f)
		{
			DispatchEvent(e);
		}
		else
		{
			m_customEvents.push(e);
		}
		m_customEvents.pop();
	}
}

void EventManager::DispatchEvent(const Event& _event)
{
	auto eRange = m_listeners.equal_range(_event.m_eventID);

	for (auto& itr = eRange.first; itr != eRange.second; ++itr)
	{
		itr->second.EventFunction(_event.m_parameter);
	}
}

bool EventManager::CheckSubscribe(std::string _eventID, const EventHandler* _listener)
{
	auto eRange = m_listeners.equal_range(_eventID);

	for (auto& itr = eRange.first; itr != eRange.second; ++itr)
	{
		if (itr->second.m_listener == _listener)
		{
			return true;
		}
	}
	return false;
}
