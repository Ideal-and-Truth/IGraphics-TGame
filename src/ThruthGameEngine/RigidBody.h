#pragma once
#include "Headers.h"
#include "Component.h"

namespace physx
{
	class PxRigidDynamic;
}

namespace Truth
{
	class RigidBody :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(RigidBody);
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	public:
		PROPERTY(mass);
		float m_mass;
		PROPERTY(drag);
		float m_drag;
		PROPERTY(angularDrag);
		float m_angularDrag;

		PROPERTY(useGravity);
		bool m_useGravity;
		PROPERTY(isKinematic);
		bool m_isKinematic;

		PROPERTY(freezePosition);
		bool m_freezePosition[3];
		PROPERTY(freezeRotation);
		bool m_freezeRotation[3];

		Vector3 m_velocity;
		physx::PxRigidDynamic* m_body;

	private:
		float m_speed;
		Vector3 m_angularVelocity;
		Vector3 m_inertiaTensor;
		Vector3 m_inertiaTensorRotation;
		Vector3 m_localMassCenter;
		Vector3 m_worldMassCenter;

		std::weak_ptr<Transform> m_transform;

		bool m_isChanged;

	public:
		RigidBody();
		virtual ~RigidBody();

		void FixedUpdate(std::any _p);

		void FreezePosition(bool _xzy[3]);
		void FreezeRotation(bool _xzy[3]);

		METHOD(Update);
		void Update() override;

		void AddImpulse(Vector3 _force);
		void SetLinearVelocity(Vector3 _val);
		Vector3 GetLinearVelocity() const;

		void InitalizeMassAndInertia();

	private:
		METHOD(Awake);
		void Awake();

		METHOD(Initalize);
		void Initalize();


		METHOD(Start);
		void Start();
	};

	template<class Archive>
	void Truth::RigidBody::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
	}
}

