#ifndef COMPOSITE_PASS_HLSL
#define COMPOSITE_PASS_HLSL

struct VSInput
{
    float4 Pos : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D Blur0 : register(t0);
Texture2D Blur1 : register(t1);

cbuffer CompositeParams : register(b0)
{
    float coefficient;
}
SamplerState g_sampler : register(s0);

PSInput VS(VSInput Input)
{
    PSInput result;
    result.position = Input.Pos;
    result.uv = Input.TexCoord;
    return result;
}

float4 PS(PSInput input) : SV_TARGET
{
    float4 Blur;
    Blur = mad(coefficient, Blur1.Sample(g_sampler, input.uv), Blur0.Sample(g_sampler, input.uv));
    return Blur;
}
#endif