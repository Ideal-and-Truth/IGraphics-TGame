#ifndef DEFAULT_DEBUG_LINE_HLSL
#define DEFAULT_DEBUG_LINE_HLSL

cbuffer Global : register(b0)
{
    float4x4 View;
    float4x4 Proj;
    float4x4 ViewProj;
};

cbuffer LineInfo : register(b1)
{
    float3 StartPos;
    float pad0;
    float3 EndPos;
    float pad1;
}

cbuffer Color : register(b2)
{
    float4 Color;
}

struct VSInput
{
    float3 Pos : POSITION;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float3 worldPosition = lerp(StartPos, EndPos, input.Pos.x);
    output.Pos = mul(float4(worldPosition, 1.f), ViewProj);
    output.Color = Color;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return input.Color;
}
#endif
