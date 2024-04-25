#pragma once
#include "Core/Core.h"

#define MAX_BONE_TRANSFORMS 50

struct BoneDesc
{
	Matrix transforms[MAX_BONE_TRANSFORMS];
};

struct alignas(256) CommonMatrix
{
	uint32 BoneIndex;
	Matrix World;
};

struct alignas(256) Transform
{
	Matrix World;
	Matrix View;
	Matrix Proj;
	Matrix WorldInvTranspose;
};