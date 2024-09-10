//#ifndef PARTICLE_COMMON_HLSLI
//#define PARTICLE_COMMON_HLSLI

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
    float4 g_CustomData1;
    float4 g_CustomData2;
    float g_Time;
    float3 pad;
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
    float4 PosH : SV_POSITION;
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

//#endif
