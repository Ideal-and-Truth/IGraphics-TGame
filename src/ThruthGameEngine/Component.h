#pragma once
#include "Headers.h"
#include "EventHandler.h"
#include "Managers.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Scene.h"

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
}



namespace Truth
{
	class Component abstract
		: public EventHandler
	{
		GENERATE_CLASS_TYPE_INFO(Component);

	public:
		std::string m_name;
		int32 m_index;

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	protected:
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
		virtual void Initalize() {};
		virtual void ApplyTransform() {};
#ifdef _DEBUG
		virtual void EditorSetValue() {};
#endif // _DEBUG


		void SetOwner(std::weak_ptr<Entity> _val) { m_owner = _val; }
		std::weak_ptr<Entity> GetOwner() const { return m_owner; }
		void SetManager(std::weak_ptr<Managers> _val) { m_managers = _val; }

#pragma region inline
	protected:
		// 자주 사용될 기능을 미리 묶어 놓는다.
		// Component 에서 자주 사용될 함수 목록
		// 이벤트 관련 함수
		template <typename T>
		inline void EventBind(std::string _name, void (T::* func)(const void*))
		{
			m_managers.lock()->Event()->Subscribe(_name, MakeListenerInfo(func));
		}
		inline void EventPublish(std::string _name, const void* _param, float4 _delay = 0.0f)
		{
			m_managers.lock()->Event()->PublishEvent(_name, _param, _delay);
		}

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

		inline int16 MouseDx()
		{
			return m_managers.lock()->Input()->GetMouseMoveX();
		}
		inline int16 MouseDy()
		{
			return m_managers.lock()->Input()->GetMouseMoveY();
		}
		// 시간 관련 함수들
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

		void SetRotation(const Quaternion& _val);
		void SetPosition(const Vector3& _pos) const;
		void SetScale(const Vector3& _scale) const;

		const Vector3& GetPosition() const;
		const Quaternion& GetRotation() const;
		const Vector3& GetScale() const;

		const Matrix& GetWorldTM() const;
		void SetWorldTM(const Matrix& _tm);

		void AddEmptyEntity();
		void AddEntity(std::shared_ptr<Entity> _entity);

#pragma endregion inline
	};
}
template<class Archive>
void Truth::Component::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& m_name;
	_ar& m_canMultiple;
}

template<class Archive>
void Truth::Component::load(Archive& _ar, const unsigned int file_version)
{
	_ar& m_name;
	_ar& m_canMultiple;
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Truth::Component)
BOOST_CLASS_EXPORT_KEY(Truth::Component)
BOOST_CLASS_VERSION(Truth::Component, 0)