#pragma once
#include "Entity.h"

namespace Truth
{
	class DefaultCamera
		: public Truth::Entity
	{
		GENERATE_CLASS_TYPE_INFO(DefaultCamera)

	public:
		DefaultCamera();
		virtual ~DefaultCamera();
		virtual void Initailize() override;
	};
}

