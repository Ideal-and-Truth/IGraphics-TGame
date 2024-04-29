#pragma once
#include "GraphicsEngine/public/IAnimation.h"
#include "GraphicsEngine/Resource/ModelAnimation.h"

namespace Ideal
{
	class IdealAnimation : public IAnimation
	{
	public:
		IdealAnimation();
		virtual ~IdealAnimation();

	public:
		std::shared_ptr<ModelKeyframe> GetKeyframe(const std::wstring& name);

		std::wstring name;
		float duration = 0.f;
		float frameRate = 0.f;
		uint32 frameCount = 0;
		std::unordered_map<std::wstring, std::shared_ptr<ModelKeyframe>> keyframes;
	};
}