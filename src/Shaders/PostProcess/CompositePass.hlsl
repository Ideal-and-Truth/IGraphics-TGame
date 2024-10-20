#ifndef COMPOSITE_PASS_HLSL
#define COMPOSITE_PASS_HLSL

// https://jongzero.tistory.com/33
// https://gist.github.com/da-molchanov/85b95ab3f20fa4eb5624f9b3b959a0ee
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
#define ACES_Filmic
//#define ACES_Filmic2
//#define Reinhard

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


// The code in this file was originally written by Stephen Hill (@self_shadow), who deserves all
// credit for coming up with this fit and implementing it. Buy him a beer next time you see him. :)

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
static const float3x3 ACESInputMat =
{
    { 0.59719, 0.35458, 0.04823 },
    { 0.07600, 0.90834, 0.01566 },
    { 0.02840, 0.13383, 0.83777 }
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367 },
    { -0.10208, 1.10813, -0.00605 },
    { -0.00327, -0.07276, 1.07602 }
};

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 ACESFitted(float3 color)
{
    color = mul(ACESInputMat, color);

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = mul(ACESOutputMat, color);

    // Clamp to [0, 1]
    color = saturate(color);

    return color;
}

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
    
#ifdef ACES_Filmic
    float3 color = Blur.rgb;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    
    color = saturate((color * (a * color + b)) / (color * (c * color + d) + e));
    color = pow(color, 1 / 2.2f);
    return float4(color.xyz, 1.f);
#endif
#ifdef ACES_Filmic2
    float3 color = Blur.rgb;
    color = ACESFitted(color);
    return float4(color, 1);
#endif
#ifdef Reinhard
    float4 color = Blur;
    //float k = 1.f;
    //color.xyz = color.xyz / (color.xyz + k);
    //color = pow(color, 1 / 2.2f);
    return color;
#endif
    
    return Blur;
}
#endif