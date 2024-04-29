#pragma once
#include "../Utils/SimpleMath.h"
#include <string>
#include <memory>

namespace Ideal
{
	class IAnimation;
}
namespace Ideal
{
	class IDynamicMeshObject
	{
	public:
		IDynamicMeshObject() {}
		virtual ~IDynamicMeshObject() {}

	public:
		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& matrix) abstract;
		virtual void AddAnimation(std::shared_ptr<Ideal::IAnimation> Animation) abstract;
	};
}