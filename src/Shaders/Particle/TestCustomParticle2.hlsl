#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

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

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    
    float4 worldPos = mul(float4(input.Pos,1), World);
    worldPos = mul(worldPos, ViewProj);
    
    output.Pos = worldPos.xyz;
    output.Normal = normalize(mul(input.Normal, (float3x3)World));
    output.UV = input.UV;

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    VSOutput output;
    output.Pos = input.Pos;    // Pos, Normal 등 필요한 값 설정
    output.Normal = input.Normal;
    output.UV = input.UV;
    
    return float4(0, 0, 0, 1); // SV_POSITION 값을 반환
}

#endif
