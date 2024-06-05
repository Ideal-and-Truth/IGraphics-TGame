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

	public:
		Camera();
		virtual ~Camera();

		METHOD(Update);
		virtual void Update();

		METHOD(SetLens);
		void SetLens(float _fovY, float _aspect, float _nearZ, float _farZ);

		void SetMainCamera();

	private:
		METHOD(Awake);
		virtual void Awake();
	};
}

