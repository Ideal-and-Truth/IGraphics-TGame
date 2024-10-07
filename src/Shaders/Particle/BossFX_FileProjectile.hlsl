#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.UV;
    float2 maskUV = input.UV;

    // �ð��� ���� �ִϸ��̼��� ���� UV ��ȯ
    float scrollSpeed = 2.5; // ��ũ�� �ӵ� ����
    //float scrollSpeed = 5; // ��ũ�� �ӵ� ����
    uv.y += g_Time * scrollSpeed; // �ð��� ���� Y�� �������� ��ũ��
    //maskUV.y += 0.8;    
    //maskUV.y += g_Time;    
    //float temp = maskUV.x;
    //maskUV.x = -maskUV.y;
    //maskUV.y = -temp;
    // ����ũ �ؽ�ó�� ȭ�� �ؽ�ó ���ø�
    float4 maskColor = ParticleTexture0.Sample(LinearWrapSampler, maskUV);
    //float4 maskColor = ParticleTexture0.Sample(LinearClampSampler, maskUV);
    //float4 maskColor = ParticleTexture0.Sample(LinearWrapSampler, uv);
    float4 fireColor = ParticleTexture1.Sample(LinearWrapSampler, uv);
    
    // ȥ�� ȿ��: ����ũ ���� ���� ȭ�� ������ ����
    float4 outputColor = g_startColor * fireColor * maskColor.a; // ���İ��� ����Ͽ� ȥ��
    float4 newMaskColor = 1 - maskColor;    
    outputColor.rgb *= newMaskColor.rgb; // ����ũ�� ������ ä���� ����Ͽ� ��� ����
    //outputColor.rgb *= maskColor.rgb;

    // ��� ���� ��ȯ
    return outputColor;
}


#endif
