#pragma once
#include "Core/Core.h"

#define MAX_BONE_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
struct CB_Bone
{
	Matrix transforms[MAX_BONE_TRANSFORMS];
};

struct CommonMatrix
{
	uint32 BoneIndex;
	Matrix World;
};

struct CB_Transform
{
	Matrix World;
	Matrix View;
	Matrix Proj;
	Matrix WorldInvTranspose;
};

struct CB_Material
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;
};