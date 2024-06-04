#pragma once
#include "Headers.h"
#include "PhysicsManager.h"

namespace MathConverter
{
	physx::PxVec3 Convert(Vector3 _val);
	physx::PxVec2 Convert(Vector2 _val);

	Vector3 Convert(physx::PxVec3 _val);

	Quaternion Convert(physx::PxQuat& _val);

	Matrix Convert(physx::PxTransform&& _val);
};

