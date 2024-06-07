#pragma once
#include "Collider.h"
#include "Headers.h"
namespace Truth
{
	class CapsuleCollider
		: public Collider
	{
		GENERATE_CLASS_TYPE_INFO(CapsuleCollider);
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	public:
		PROPERTY(radius);
		float m_radius;
		PROPERTY(height);
		float m_height;

	public:
		CapsuleCollider(bool _isTrigger = true);
		CapsuleCollider(float _radius, float _height, bool _isTrigger = true);
		CapsuleCollider(Vector3 _pos, float _radius, float _height, bool _isTrigger = true);
		virtual ~CapsuleCollider();

		void SetRadius(float _radius);
		void SetHeight(float _height);

	private:
		METHOD(Awake);
		void Awake();


		METHOD(Initalize);
		void Initalize();
	};

	template<class Archive>
	void Truth::CapsuleCollider::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Collider>(*this);
		_ar& m_radius;
		_ar& m_height;
	}

}

