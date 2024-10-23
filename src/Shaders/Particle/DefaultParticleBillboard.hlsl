#ifndef DEFAULT_PARTICLE_Billboard_CS_HLSL
#define DEFAULT_PARTICLE_Billboard_CS_HLSL

#include "ParticleCommon.hlsli" 

[numthreads(256, 1, 1)]
void ParticleBillboardCSMain(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    // TEMP Particle MaxCount
    uint maxParticleCount = 100;
    if(index >= maxParticleCount)
    {
        return;
    }
    
    // TEMP
    float radius = 7.5f; // 원하는 반지름
    float radiusThickness = 0.5f;
}

#endif
