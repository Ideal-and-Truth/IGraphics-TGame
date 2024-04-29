#include "IdealAnimation.h"

Ideal::IdealAnimation::IdealAnimation()
{

}

Ideal::IdealAnimation::~IdealAnimation()
{

}

std::shared_ptr<Ideal::ModelKeyframe> Ideal::IdealAnimation::GetKeyframe(const std::wstring& name)
{
	auto findIt = keyframes.find(name);
	if (findIt == keyframes.end())
		return nullptr;
	return findIt->second;
}
