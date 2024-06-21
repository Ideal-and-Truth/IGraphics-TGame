#pragma once
#include "IMeshObject.h"
#include "../Utils/SimpleMath.h"
#include <string>
#include <memory>

namespace Ideal
{
	class IAnimation;
}
namespace Ideal
{
	class ISkinnedMeshObject : public IMeshObject
	{
	public:
		ISkinnedMeshObject() {}
		virtual ~ISkinnedMeshObject() {}

	public:
		virtual void AddAnimation(const std::string& AnimationName, std::shared_ptr<Ideal::IAnimation> Animation) abstract;
		virtual void SetAnimation(const std::string& AnimationName, bool WhenCurrentAnimationFinished = true) abstract;
		virtual uint32 GetCurrentAnimationIndex() abstract;
	};
}