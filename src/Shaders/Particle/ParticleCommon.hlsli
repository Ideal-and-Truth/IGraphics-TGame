#ifndef PARTICLE_COMMON_HLSL
#define PARTICLE_COMMON_HLSL

cbuffer Global : register(b0)
{
    float4x4 View;
    float4x4 Proj;
    float4x4 ViewProj;
};

cbuffer Transform : register(b1)
{
    float4x4 World;
    float4x4 WorldInvTranspose;
}
cbuffer ParticleSystemData : register(b2)
{
    // Time
    float g_Time;
    float4 g_CustomData1;
    float4 g_CustomData2;
};

// Noise Texture?

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

#endif
