#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Managers.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Scene.h"

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
}



namespace Truth
{
	class Component
		: public EventHandler
	{
		GENERATE_CLASS_TYPE_INFO(Component)

	protected:
		PROPERTY(canMultiple);
		bool m_canMultiple;

		std::weak_ptr<Entity> m_owner;
		std::weak_ptr<Transform> m_transform;
		std::weak_ptr<Managers> m_managers;

	public:
		Component();
		virtual ~Component();

		bool CanMultiple() const { return m_canMultiple; }

		virtual void Update() {};
		virtual void Awake() {};
		virtual void Start() {};
		virtual void LateUpdate() {};
		virtual void FixedUpdate() {};
		virtual void OnCollisionEnter() {};
		virtual void OnCollisionExit() {};
		virtual void OnCollisionStay() {};
		virtual void OnTriggerEnter() {};
		virtual void OnTriggerExit() {};
		virtual void OnTriggerStay() {};


		void SetOwner(std::weak_ptr<Entity> _val) { m_owner = _val; }
		std::weak_ptr<Entity> GetOwner() const { return m_owner; }
		void SetManager(std::weak_ptr<Managers> _val) { m_managers = _val; }

#pragma region inline
	protected:
		// ���� ���� ����� �̸� ���� ���´�.
		// Component ���� ���� ���� �Լ� ���
		// �̺�Ʈ ���� �Լ�
		template <typename T>
		inline void EventBind(std::string _name, void (T::* func)(std::any))
		{
			m_managers.lock()->Event()->Subscribe(_name, MakeListenerInfo(func));
		}
		inline void EventPublish(std::string _name, std::any _param, float4 _delay = 0.0f)
		{
			m_managers.lock()->Event()->PublishEvent(_name, _param, _delay);
		}

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

		inline int16 MouseDx()
		{
			return m_managers.lock()->Input()->GetMouseMoveX();
		}
		inline int16 MouseDy()
		{
			return m_managers.lock()->Input()->GetMouseMoveY();
		}
		// �ð� ���� �Լ���
		inline float GetDeltaTime()
		{
			return m_managers.lock()->Time()->GetDT();
		}

		inline float GetFixedDeltaTime()
		{
			return m_managers.lock()->Time()->GetFDT();
		}
	public:
		void Translate(Vector3& _val);

		void SetPosition(Vector3& _val);
		void SetRotation(Quaternion& _val);


		template <typename E>
		void AddEntity();
#pragma endregion inline
	};
}

template <typename E>
void Truth::Component::AddEntity()
{
	m_managers.lock()->Scene()->m_currentScene.lock()->CreateEntity(std::make_shared<E>());
}