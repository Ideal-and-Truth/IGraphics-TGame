#pragma once
#include "Core/Core.h"

#define MAX_BONE_TRANSFORMS 600
#define MAX_MODEL_KEYFRAMES 600


struct SceneConstantBuffer
{
	Matrix View;
	Matrix Proj;
	Matrix ProjToWorld;
	DirectX::XMVECTOR CameraPos;

	uint32 maxRadianceRayRecursionDepth;
	uint32 maxShadowRayRecursionDepth;
	float nearZ;
	float farZ;
};

struct CB_MaterialInfo
{
	uint32 bUseDiffuseMap;
	uint32 bUseNormalMap;
	uint32 bUseMetallicMap;
	uint32 bUseRoughnessMap;

	float metallicFactor;
	float roughnessFactor;

	Vector2 tiling;
	Vector2 offset;

	float pad0;
	float pad1;
};

struct CB_Bone
{
	Matrix transforms[MAX_BONE_TRANSFORMS];
};

struct CB_Color
{
	Color color;
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

struct CB_DebugLine
{
	Vector3 startPos;
	float pad0;
	Vector3 endPos;
	float pad1;
	Color color;
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
		PosOffset = Vector2(0, 0);
		//pad0 = 0;
		//pad1 = 0;
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
	Vector2 PosOffset;
	//float pad0;
	//float pad1;
};

//----Particle----//
struct CB_ParticleSystem
{
	float CustomData1[4];
	float CustomData2[4];
	float Time = 0.f;
	Vector3 pad;
	Color StartColor;
};