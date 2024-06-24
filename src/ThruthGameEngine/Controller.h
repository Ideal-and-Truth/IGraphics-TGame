#pragma once
#include "Component.h"

namespace physx
{
	class PxController;
}

namespace Truth
{
	class RigidBody;
}

namespace Truth
{
	class Controller
		: public Component
	{
		GENERATE_CLASS_TYPE_INFO(Controller);

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	private:
		physx::PxController* m_controller;
		std::shared_ptr<RigidBody> m_rigidbody;

		bool m_isJumping;

		float m_jumpStrength;
		float m_jumpVelocity;

		float m_velocity;

		float m_moveSpeed;

		float m_gravity;

		const float m_minmumDistance;

		uint32 m_flag;

	public:
		Controller();
		~Controller();

		METHOD(Initalize);
		virtual void Initalize() override;

		METHOD(Start);
		virtual void Start() override;

		void Move(Vector3& _disp);
		bool SetPosition(Vector3& _disp);
		void SetRotation(Quaternion& _val);
		bool IsCollisionDown();
		bool IsCollisionUp();
		bool IsCollisionSide();
	};

	template<class Archive>
	void Truth::Controller::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}
}
BOOST_CLASS_EXPORT_KEY(Truth::Controller)