#pragma once
//#include "GraphicsEngine/SimpleMath.h"
#include "../Utils/SimpleMath.h"

namespace Ideal
{
	class IMeshObject
	{
	public:
		IMeshObject() {}
		virtual ~IMeshObject() {}

	public:
		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& matrix) abstract;
	};
}