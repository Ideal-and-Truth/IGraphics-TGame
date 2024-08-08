#ifndef UI_HLSL
#define UI_HLSL


Texture2D texture : register(t0);
SamplerState WrapLinearSampler : register(s0);

cbuffer CB_Sprite : register(b0)
{
    float2 g_ScreenRes;
    float2 g_Pos;
    float2 g_Scale;
    float2 g_TexSize;
    float2 g_TexSamplePos;
    float2 g_TexSampleSize;
    float g_Z;
    float g_Alpha;
    float pad0;
    float pad1;
}

struct VSInput
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

struct PSInput
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
}

PSInput VS(VSInput Input)
{
    PSInput result = (PSInput)0;
    
    float2 scale = (g_TexSize / g_ScreenRes) * g_Scale;
    float2 offset = (g_Pos / g_ScreenRes);  // float ��ǥ�� ���� ������ ��ġ
    float2 pos = Input.Pos.xy * scale + offset;
    result.Pos = float4(Pos.xy * float2(2, -2) + float2(-1, 1), g_Z, 1); // ���� ��ǥ��� ��ȯ

    float2 texScale = (g_TexSampleSize / g_TexSize);
    float2 texOffset = (g_TexSamplePos / g_TexSize);
    result.TexCoord = Input.TexCoord * texScale + texOffset;
    
    result.Color = Input.Color;
    return result;
}

float4 PS(PSInput Input) : SV_Target
{
    float4 texColor = texture.Sample(WrapLinearSampler, Input.TexCoord);
    return texColor * Input.Color;
}

#endif