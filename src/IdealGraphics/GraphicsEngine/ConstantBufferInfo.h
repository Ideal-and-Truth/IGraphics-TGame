#pragma once
#include "Core/Core.h"

#define MAX_BONE_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500


struct SceneConstantBuffer
{
	Matrix ProjToWorld;
	DirectX::XMVECTOR CameraPos;
	DirectX::XMVECTOR lightPos;
	DirectX::XMVECTOR lightAmbient;
	DirectX::XMVECTOR lightDiffuse;

	uint32 maxRadianceRayRecursionDepth;
	uint32 maxShadowRayRecursionDepth;
	DirectX::XMVECTOR color;
	float padding[2];
};

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
	Vector3 eyePos;
};

//------------Light-----------//
#define MAX_POINT_LIGHT_NUM 16
#define MAX_SPOT_LIGHT_NUM 16

struct PointLight
{
	Color Color;
	Vector3 Position;
	float Range;
	float Intensity;
	float pad[3];
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
	float pad[2];
};

struct CB_LightList
{
	int32 PointLightNum;
	int32 SpotLightNum;
	float pad[2];
	DirectionalLight DirLight;
	PointLight PointLights[MAX_POINT_LIGHT_NUM];
	SpotLight SpotLights[MAX_SPOT_LIGHT_NUM];
};