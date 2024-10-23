#ifndef CS_DEFAULT_PARTICLE_Billboard_HLSL
#define CS_DEFAULT_PARTICLE_Billboard_HLSL

#include "ParticleCommon.hlsli" 

float Random(uint id) {
    uint seed = 236623469;
    // ������ ���� ������
    seed ^= id;
    seed *= 0x343FD;
    seed += 0x269EC3;
    return frac((float)(seed) / 0xFFFFFFFF);
}
#define PI 3.14159265359f
[numthreads(256, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    // TEMP Particle MaxCount
    uint maxParticleCount = 100;
    if(index >= maxParticleCount)
    {
        return;
    }
    
    // TEMP
    float radius = 7.5f; // ���ϴ� ������
    float radiusThickness = 0.5f;
    
    float angle = Random(index) * 2.f * PI; // ���� ���� [0, 2pi]
    float distance = (Random(index + 1) * radius) % radiusThickness;
    
    float x = distance * cos(angle);
    float y = distance * sin(angle);
    float3 position = float3(x,y,0.f);  // 2d ��, z�� 0�̿����Ѵ�

    // TODO UAV ����
    g_RWBufferPos[index].pos = float4(position, 1.f);
}

#endif
