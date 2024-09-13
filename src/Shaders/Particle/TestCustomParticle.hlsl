#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 scroll = float2(0.2, 0.0);
    float2 scrolledUV = input.UV + scroll * g_Time;
    // UV Ÿ�ϸ� �� ������
    float2 uvTiled;
    {
        float2 offset = scroll * g_Time;
        Ideal_TilingAndOffset_float(input.UV, float2(1.0, 1.0), offset, uvTiled);
    }   
    // Simple ������ ����
    float noise;
    {
        Ideal_SimpleNoise_float(uvTiled, 50.0, noise);
    }
    // Step �Լ� ����
    float4 stepData;
    {
        //float4 edge = float4(noise, noise, noise, noise);
        //float4 InData = float4(0.5, 0.5, 0.5, 0.5);
        //Step_float4(edge, InData, stepData);
        float tempTime = 1 - g_Time;
        float s = step(noise, tempTime);
        stepData = float4(s,s,s,s);
    }
    
    // Step �Լ� ���� (������ ��� ����ũ)
    //float4 edge = float4(0.5, 0.5, 0.5, 1.0);
    //float4 noiseMask;
    //Step_float4(edge, float4(noise, noise, noise, 1.0), noiseMask);
    //
    //// �ؽ�ó ���ø�
    //float4 texColor = ParticleTexture.Sample(LinearWrapSampler, scrolledUV);

    // ���� ���� ��� (������� �ؽ�ó ����)
    //return lerp(texColor, float4(1.0, 1.0, 1.0, 1.0), noiseMask);
    //return noiseMask;
    //return texColor;

    float dis_amount = 0.2f;
    float gradientNoise;
    Ideal_GradientNoise_float(scrolledUV, 10, gradientNoise);
    
    float2 outValue = lerp(input.UV, float2(gradientNoise, gradientNoise), dis_amount);
    float4 SampleTexture = ParticleTexture.Sample(LinearClampSampler, outValue);
    //float4 SampleTexture = ParticleTexture.Sample(LinearClampSampler, input.UV);



    float4 data = mul(stepData, SampleTexture);
    data = mul(float4(1, 1, 1, 1), data);
    
    // custom color
    data = mul(data, float4(0.4, 0, 0, 1));
    return data;
    












    //float time = 1.f;
    //float4 customColor = float4(1,0,0,1);
    //float2 scroll = float2(0.2, 0);
    //float dis_amount = 1;
    //
    //
    //float2 Out;
    //TilingAndOffset_float(input.UV, float2(1,1), scroll, Out);
    //SimpleNoise_float(Out, 50, Out);
    //step(Out, 0);
    //
    //
    //float4 color = ParticleTexture.Sample(LinearWrapSampler, input.UV);
    //if(color.x < 0.001 && color.y < 0.001 && color.z < 0.001)
    //    color.w = 0.f;
    //return color;
    //return float4(1, 0, 0, 0.5); // SV_POSITION ���� ��ȯ
}

#endif
