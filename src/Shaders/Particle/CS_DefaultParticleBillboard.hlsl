#ifndef CS_DEFAULT_PARTICLE_Billboard_HLSL
#define CS_DEFAULT_PARTICLE_Billboard_HLSL

#include "ParticleCommon.hlsli" 

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
    
   
    // TODO UAV ¿¬°á
    g_RWBufferPos[index].Position = float4(position, 1.f);
}

#endif
