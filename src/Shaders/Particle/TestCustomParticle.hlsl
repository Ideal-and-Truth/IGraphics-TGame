#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    VSOutput output;
    output.Pos = input.Pos;    // Pos, Normal �� �ʿ��� �� ����
    output.Normal = input.Normal;
    output.UV = input.UV;
    
    return float4(0, 0, 0, 1); // SV_POSITION ���� ��ȯ
}

#endif
