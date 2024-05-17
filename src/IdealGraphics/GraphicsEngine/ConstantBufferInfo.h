#pragma once
#include "Core/Core.h"

#define MAX_BONE_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
struct CB_Bone
{
	Matrix transforms[MAX_BONE_TRANSFORMS];
};

struct CB_Transform
{
	Matrix World;
	Matrix WorldInvTranspose;
};

struct CB_Material
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;
};

struct CB_Global
{
	Matrix View;
	Matrix Proj;
	Matrix ViewProj;
};

//------------Light-----------//
#define MAX_POINT_LIGHT_NUM 1024
#define MAX_SPOT_LIGHT_NUM 1024

struct PointLight
{
	Color Color;
	Vector3 Position;
	float Range;
	float Intensity;
};

struct DirectionalLight
{
	Color AmbientColor;
	Color DiffuseColor;
	Vector3 Direction;
	float Intensity;
};

struct SpotLight
{
	Color Color;
	Vector4 Direction;
	Vector3 Position;
	float SpotAngle;
	float Range;	
	float Intensity;
};

struct CB_LightList
{
	PointLight PointLights[MAX_POINT_LIGHT_NUM];
	SpotLight SpotLights[MAX_SPOT_LIGHT_NUM];
};