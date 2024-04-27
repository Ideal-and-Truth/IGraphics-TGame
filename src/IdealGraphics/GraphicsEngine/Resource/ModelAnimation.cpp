#include "GraphicsEngine/Resource/ModelAnimation.h"

using namespace Ideal;

std::shared_ptr<ModelKeyframe> ModelAnimation::GetKeyframe(const std::wstring& name)
{
	auto findIt = keyframes.find(name);
	if (findIt == keyframes.end())
		return nullptr;
	return findIt->second;
}
