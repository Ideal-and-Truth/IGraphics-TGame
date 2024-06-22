#pragma once
#include "IRendererResource.h"
#include "../Utils/SimpleMath.h"

namespace Ideal
{
	class IMeshObject : public IRendererResource
	{
	public:
		IMeshObject() {}
		virtual ~IMeshObject() {}

	public:
		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& Matrix) abstract;
		virtual void SetDrawObject(bool IsDraw) abstract;
	};
}