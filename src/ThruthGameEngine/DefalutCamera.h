#pragma once
#include "Entity.h"

namespace Truth
{
	class DefalutCamera
		: public Truth::Entity
	{
		GENERATE_CLASS_TYPE_INFO(DefalutCamera)

	public:
		DefalutCamera();
		virtual ~DefalutCamera();
		virtual void Initailize() override;
	};
}

