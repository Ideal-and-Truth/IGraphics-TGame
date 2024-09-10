#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    VSOutput output;
    output.Pos = input.Pos;    // Pos, Normal 등 필요한 값 설정
    output.Normal = input.Normal;
    output.UV = input.UV;
    
    return float4(0, 0, 0, 1); // SV_POSITION 값을 반환
}

#endif
