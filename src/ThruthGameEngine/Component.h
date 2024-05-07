#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Managers.h"
#include "InputManager.h"

/// <summary>
/// ��� ������Ʈ�� �θ� Ŭ����
/// �ϴ��� �̷��� �����غ���...
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

		// ���� ���� ����� �̸� ���� ���´�.
		// Component ���� ���� ���� �Լ� ���
	protected:
		// ������ �ð� ��������
		float4 GetDeltaTime();
		float4 GetFixedDeltaTime();

		// �̺�Ʈ ���� �Լ�
		template <typename T>
		void EventBind(std::string _name, void (T::* func)(std::any));
		void EventPublish(std::string _name, std::any _param = nullptr, float4 _delay = 0.0f);

		// �Է� ���� �Լ�
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

