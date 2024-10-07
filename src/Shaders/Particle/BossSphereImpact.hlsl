#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{  // �⺻ ����
     // �⺻ ����
    float4 baseColor = ParticleTexture0.Sample(LinearWrapSampler, input.UV);
    
    baseColor.rgb *= g_startColor.rgb;
    baseColor.rgb = baseColor.rgb / (baseColor.rgb + float3(1, 1, 1)); // �� ���� ���� (Reinhard �� ����)

    baseColor.a *= g_startColor.a;

    // �ð��� ���� �ִϸ��̼� ���� (Texture Animate)
    float2 animatedUV = input.UV;
    float2 animatedUVTO;
    Ideal_TilingAndOffset_float(input.UV, float2(5, 2), float2(0, 0), animatedUVTO);
    animatedUVTO += g_Time * g_CustomData1.x; // �ִϸ��̼� �ӵ� ����
    baseColor *= ParticleTexture0.Sample(LinearWrapSampler, animatedUVTO); // �ؽ�ó �ִϸ��̼� ����

    // ��� �ְ� (Normal Distortion)
    float2 normalUV;
    Ideal_TilingAndOffset_float(input.UV, float2(2, 2), float2(0, 0), normalUV);
    float3 normal = ParticleTexture1.Sample(LinearWrapSampler, normalUV).rgb; // ��� �� ���
    normal = normalize(normal * 2.0 - 1.0); // [-1, 1] ������ ��ȯ

    // ����ũ ���̵� (Mask Fade)
    float2 maskUV;
    Ideal_TilingAndOffset_float(input.UV, float2(1, -1), float2(0, 0), maskUV);
    float maskAlpha = ParticleTexture2.Sample(LinearWrapSampler, maskUV).a; // ����ũ �ؽ�ó ���

    // Y���� �������� �ϴ� �ݱ��� ����
    float isInLowerHemisphere = step(0.0, -input.Pos.y); // Y�� 0���� ������ 1, �ƴϸ� 0
    baseColor.a *= maskAlpha * isInLowerHemisphere; // ���� �� ����

    // �ϴ� �ݱ����� ����ǵ��� �߰� ����
    baseColor.rgb *= isInLowerHemisphere; // ���� �ϴ� �ݱ������� ���̵��� ����
    
    // ���� ���� ��ȯ
    return baseColor;
}


#endif
