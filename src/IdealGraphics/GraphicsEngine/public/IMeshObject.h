#pragma once
#include "SimpleMath.h"

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