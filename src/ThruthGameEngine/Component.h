#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Managers.h"
#include "InputManager.h"

/// <summary>
/// 모든 컴포넌트의 부모 클래스
/// 일단은 이렇게 구현해보자...
/// </summary>

enum class KEY;

namespace Truth
{
	class Entity;
	class Transform;
	class Managers;

	class Component
		: public EventHandler
	{
		GENERATE_CLASS_TYPE_INFO(Component)

	protected:
		PROPERTY(canMultiple)
			bool m_canMultiple;

		std::weak_ptr<Entity> m_owner;
		std::weak_ptr<Transform> m_transform;
		std::weak_ptr<Managers> m_managers;

	public:
		Component();
		virtual ~Component();

		virtual void Awake() abstract;

		bool CanMultiple() const { return m_canMultiple; }

		void SetOwner(std::weak_ptr<Entity> _val) { m_owner = _val; }

		void SetManager(std::weak_ptr<Managers> _val) { m_managers = _val; }

		// 자주 사용될 기능을 미리 묶어 놓는다.
		// Component 에서 자주 사용될 함수 목록
	protected:
		// 프레임 시간 가져오기
		float4 GetDeltaTime();
		float4 GetFixedDeltaTime();

		// 이벤트 관련 함수
		template <typename T>
		void EventBind(std::string _name, void (T::* func)(std::any));
		void EventPublish(std::string _name, std::any _param = nullptr, float4 _delay = 0.0f);

		// 입력 관련 함수
		inline bool GetKeyDown(KEY _key) const
		{
			return (m_managers.lock()->Input()->GetKeyState(_key) == KEY_STATE::DOWN);
		}
		inline bool GetKeyUp(KEY _key) const
		{
			return (m_managers.lock()->Input()->GetKeyState(_key) == KEY_STATE::UP);
		}
		inline bool GetKey(KEY _key) const
		{
			return (m_managers.lock()->Input()->GetKeyState(_key) == KEY_STATE::HOLD);
		}
	};
}

template <typename T>
void Truth::Component::EventBind(std::string _name, void (T::* func)(std::any))
{
	m_managers.lock()->Event()->Subscribe(_name, MakeListenerInfo(func));
}

