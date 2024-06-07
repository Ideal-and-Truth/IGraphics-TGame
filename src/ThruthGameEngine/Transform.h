#pragma once
#include "Headers.h"
#include "Component.h"

namespace Truth
{
	class Transform :
		public Truth::Component
	{
		GENERATE_CLASS_TYPE_INFO(Transform);
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	public:
		PROPERTY(position);
		Vector3 m_position;
		PROPERTY(scale);
		Vector3 m_scale;

		Quaternion m_rotation;

		Matrix m_transformMatrix;

		Vector3 m_look;

	public:
		Transform();
		virtual ~Transform();

		// 변환 내용 적용
		void ApplyTransform();

	public:

#pragma region Inline
#pragma region Transform
		// 변환 작업 (더하기)
		inline void Translate(Vector3 _movement)
		{
			m_position += _movement;
		}

		inline void AddRotate(Vector3 _dgree)
		{
			m_rotation *= Quaternion::CreateFromYawPitchRoll(_dgree);
		}
		inline void AddRotate(Quaternion _dgree)
		{
			m_rotation *= _dgree;
		}

		inline void AddScale(Vector3 _rate)
		{
			m_scale.x *= _rate.x; m_scale.y *= _rate.y; m_scale.z *= _rate.z;
		}
		inline void AddScale(float4 _rate)
		{
			m_scale *= _rate;
		}

		// 변환 작업 (고정값)
		inline void SetPosition(Vector3 _position)
		{
			m_position = _position;
		}

		inline void SetRotate(Vector3 _dgree)
		{
			m_rotation = Quaternion::CreateFromYawPitchRoll(_dgree);
		}
		inline void SetRotate(Quaternion _dgree)
		{
			m_rotation = _dgree;
		}

		inline void SetScale(Vector3 _rate)
		{
			m_scale = _rate;
		}
		inline void SetScale(float4 _rate)
		{
			m_scale = { _rate, _rate, _rate };
		}
#pragma endregion Transform

		// 오일러각 제공 
		inline Vector3 GetEulerRotation() const
		{
			return m_rotation.ToEuler();
		}
#pragma endregion Inline
	};

	template<class Archive>
	void Truth::Transform::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_position;
		_ar& m_scale;
		_ar& m_rotation;
		_ar& m_transformMatrix;
		_ar& m_look;
	}

}

