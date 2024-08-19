#pragma once
#include "Core/Core.h"

#define MAX_BONE_TRANSFORMS 350
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

struct CB_MaterialInfo
{
	uint32 bUseDiffuseMap;
	uint32 bUseNormalMap;
	uint32 bUseMetallicMap;
	uint32 bUseRoughnessMap;

	float metallicFactor;
	float roughnessFactor;
	float pad0;
	float pad1;
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
	float pad0;
	float pad1;
	float pad2;
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
	Vector3 Direction;
	float SpotAngle;
	Vector3 Position;
	float Range;	
	float Intensity;
	float Softness;
	float pad1;
	float pad2;
};

struct CB_LightList
{
	int32 PointLightNum;
	int32 SpotLightNum;
	float pad;
	float pad1;

	DirectionalLight DirLight;
	PointLight PointLights[MAX_POINT_LIGHT_NUM];
	SpotLight SpotLights[MAX_SPOT_LIGHT_NUM];
};

//----Sprite----//
struct CB_Sprite
{
	CB_Sprite()
	{
		SpriteColor = Color(1, 1, 1, 1);
		ScreenSize = Vector2(0, 0);
		Pos = Vector2(0, 0);
		Scale = Vector2(1, 1);
		TexSize = Vector2(1, 1);
		TexSamplePos = Vector2(0, 0);
		TexSampleSize = Vector2(0, 0);
		Z = 0;
		Alpha = 1.f;
		pad0 = 0;
		pad1 = 0;
	}
	Color SpriteColor;
	Vector2 ScreenSize;
	Vector2 Pos;
	Vector2 Scale;
	Vector2 TexSize;
	Vector2 TexSamplePos;
	Vector2 TexSampleSize;
	float Z;
	float Alpha;
	float pad0;
	float pad1;
};