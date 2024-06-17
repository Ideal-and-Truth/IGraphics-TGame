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

	public:
		Camera();
		virtual ~Camera();

		METHOD(Update);
		virtual void Update();

		METHOD(SetLens);
		void SetLens(float _fovY, float _aspect, float _nearZ, float _farZ);

		void SetMainCamera();

		METHOD(Initalize);
		virtual void Initalize();

	private:
		METHOD(Awake);
		virtual void Awake();

		METHOD(Start);
		virtual void Start();

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
BOOST_CLASS_EXPORT_KEY(Truth::Camera)