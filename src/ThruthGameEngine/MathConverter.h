#pragma once
#include "Headers.h"
#include "PhysicsManager.h"

namespace MathConverter
{
	physx::PxVec3 Convert(const Vector3& _val);
	physx::PxExtendedVec3 ConvertEx(const Vector3& _val);
	physx::PxVec2 Convert(const Vector2& _val);

	Vector3 Convert(const physx::PxVec3& _val);
	Vector3 Convert(const physx::PxExtendedVec3& _val);

	Quaternion Convert(const physx::PxQuat& _val);
	physx::PxQuat Convert(const Quaternion& _val);

	Matrix Convert(const physx::PxTransform& _val);
	physx::PxTransform Convert(Matrix _val);
};

