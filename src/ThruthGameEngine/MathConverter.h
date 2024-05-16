#pragma once
#include "Headers.h"
#include "PhysicsManager.h"

namespace MathConverter
{
	physx::PxVec3 Convert(Vector3 _val);
	physx::PxVec2 Convert(Vector2 _val);
};

