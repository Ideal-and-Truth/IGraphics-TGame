#ifndef _LIGHT_HLSLI
#define _LIGHT_HLSLI

#define MAX_POINT_LIGHT_NUM 1024
#define MAX_SPOT_LIGHT_NUM 1024

struct DirectionalLight
{
	float4 Color;
	float Intensity;
	float3 Direction;
};

struct PointLight
{
	float4 Color;
	float3 Position;
	float Range;
	float Intensity;
};

struct SpotLight
{
	float4 Color;
	float4 Direction;
	float3 Position;
	float SpotAngle;
	float Range;
	float Intensity;
};

cbuffer LightList : register(b0)	// temp b0
{
	PointLight PointLights[MAX_POINT_LIGHT_NUM];
	SpotLight SpotLights[MAX_SPOT_LIGHT_NUM];
};
