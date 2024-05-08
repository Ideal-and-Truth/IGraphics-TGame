#pragma once
#include "Component.h"

namespace Ideal
{
	class ICamra;
}

namespace Truth
{
	class Camera :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Camera)

	private:
		std::shared_ptr<Ideal::ICamera> m_camera;

	public:
		Camera(std::shared_ptr<Managers> _managers);
		virtual ~Camera();

		void Update(std::any _p);

		void SetLens(float _fovY, float _aspect, float _nearZ, float _farZ);

		void SetMainCamera();
	};
}

