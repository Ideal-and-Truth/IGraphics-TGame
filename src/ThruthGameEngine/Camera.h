#pragma once
#include "Component.h"
namespace Truth
{
	class Camera :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Camera)

	private:

	public:
		Camera();
		~Camera();

		void Awake();

		void Update(std::any _p);
	};
}

