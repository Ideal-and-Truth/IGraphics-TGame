#pragma once
#include "Component.h"
#include "Headers.h"

class Truth::Transform;

namespace Truth
{
	class RigidBody :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(RigidBody)

	public:
		float m_mass;
		float m_drag;
		float m_angularDrag;

		bool m_useGravity;
		bool m_isKinematic;

		// 	bool m_freezePosition;
		// 	bool m_freezeRotation;

	private:
		float m_speed;
		Vector3 m_velocity;
		Vector3 m_angularVelocity;
		Vector3 m_inertiaTensor;
		Vector3 m_inertiaTensorRotation;
		Vector3 m_localMassCenter;
		Vector3 m_worldMassCenter;

		std::weak_ptr<Transform> m_transform;

	public:
		RigidBody();
		~RigidBody();

		void Awake() override;
	};
}

