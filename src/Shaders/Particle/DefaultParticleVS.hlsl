#ifndef DEFAULT_PARTICLE_VS_HLSL
#define DEFAULT_PARTICLE_VS_HLSL

#include "ParticleCommon.hlsli" // necessary

//cbuffer Global : register(b0)
//{
//    float4x4 View;
//    float4x4 Proj;
//    float4x4 ViewProj;
//};
//
//cbuffer Transform : register(b1)
//{
//    float4x4 World;
//    float4x4 WorldInvTranspose;
//}
//cbuffer ParticleSystemData : register(b2)
//{
//    // Time
//    float g_Time;
//    float4 g_CustomData1;
//    float4 g_CustomData2;
//};
//
//// Noise Texture?
//
//struct VSInput
//{
//    float3 Pos : POSITION;
//    float3 Normal : NORMAL;
//    float2 UV : TEXCOORD;
//};
//
//struct VSOutput
//{
//    float3 Pos : POSITION;
//    float3 Normal : NORMAL;
//    float2 UV : TEXCOORD;
//};

VSOutput Main(VSInput input)
{
    VSOutput output;
    
    float4 worldPos = mul(float4(input.Pos,1), World);
    output.Pos = worldPos.xyz;
    worldPos = mul(worldPos, ViewProj);
    output.PosH = worldPos;
    output.Normal = normalize(mul(input.Normal, (float3x3)World));
    //output.UV = input.UV;
    //output.Pos = float3(0,0,0);
    //output.Normal = float3(1,0,0);
    //output.UV = input.UV;
    return output;
}
#endif
