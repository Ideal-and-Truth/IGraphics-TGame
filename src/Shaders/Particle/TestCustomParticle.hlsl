#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 scroll = float2(0.2, 0.0);
    float2 scrolledUV = input.UV + scroll * g_Time;
    // UV 타일링 및 오프셋
    float2 uvTiled;
    {
        float2 offset = scroll * g_Time;
        Ideal_TilingAndOffset_float(input.UV, float2(1.0, 1.0), offset, uvTiled);
    }   
    // Simple 노이즈 적용
    float noise;
    {
        Ideal_SimpleNoise_float(uvTiled, 50.0, noise);
    }
    // Step 함수 적용
    float stepData;
    {
      
        float tempTime = 1 - g_Time;
        float s = step(noise, tempTime);
        stepData = s;
    }

    float dis_amount = 0.2f;
    float gradientNoise;
    Ideal_GradientNoise_float(scrolledUV, 10, gradientNoise);
    
    //float2 outValue = lerp(input.UV, float2(gradientNoise, gradientNoise), dis_amount);
    float2 outValue = lerp(scrolledUV, gradientNoise, dis_amount);
    //float4 SampleTexture = ParticleTexture.Sample(LinearClampSampler, outValue);
    float4 SampleTexture = ParticleTexture.Sample(LinearClampSampler, input.UV);
    float4 data = mul(stepData, SampleTexture);
    data = mul(float4(1, 1, 1, 0), data);
    data = mul(float4(1, 1, 1, 0), data);
    data = mul(float4(0.9245, 0.3968, 0.7052, 0), data);
    return data;
}

#endif
