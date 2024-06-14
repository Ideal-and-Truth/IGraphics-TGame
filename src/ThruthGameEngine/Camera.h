#pragma once
#include "Component.h"

namespace Ideal
{
	class ICamra;
}

namespace Truth
{
	class Camera
		: public Component
	{
		GENERATE_CLASS_TYPE_INFO(Camera)

	private:
		std::shared_ptr<Ideal::ICamera> m_camera;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

		float m_fov;
		float m_aspect;
		float m_nearZ;
		float m_farZ;

		PROPERTY(position)
		Vector3 m_position;

		PROPERTY(look)
		Vector3 m_look;

	public:
		Camera();
		virtual ~Camera();

		METHOD(Start);
		virtual void Start();

		METHOD(Update);
		virtual void Update();

		METHOD(SetLens);
		void SetLens(float _fovY, float _aspect, float _nearZ, float _farZ);

		void SetMainCamera();

		METHOD(SetLook);
		void SetLook(Vector3 _val);

		METHOD(Pitch);
		void Pitch(float angle);

		METHOD(RotateY);
		void RotateY(float angle);

	private:
		METHOD(Awake);
		virtual void Awake();
	};

	template<class Archive>
	void Truth::Camera::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_fov;
		_ar& m_aspect;
		_ar& m_nearZ;
		_ar& m_farZ;
	}
}
