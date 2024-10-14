#ifndef DEFAULT_PARTICLE_BILLBOARD_VS_HLSL
#define DEFAULT_PARTICLE_BILLBOARD_VS_HLSL

#include "ParticleCommon.hlsli" // necessary

VSParticleDrawOut VSMain(VSParticleInput input)
{
    VSParticleDrawOut output;
    output.Pos = float3(0,0,0);
    output.Color = float4(1,1,1,1);
    return output;
}

[maxvertexcount(4)]
void GSParticleDraw(point VSParticleDrawOut input[1], inout TriangleStream<GSParticleDrawOut> SpriteStream)
{
    GSParticleDrawOut output;
   
    for(int i = 0; i < 4; i++)
    {
        float3 position = g_positions[i];
        position = mul(position, (float3x3)WorldInvTranspose) + input[0].Pos;
        output.Pos = mul(float4(position, 1.0), mul(World, ViewProj));
        output.Color = input[0].Color;
        output.UV = g_texcoords[i];
        SpriteStream.Append(output);
    }
    SpriteStream.RestartStrip();

}

// TEST
float4 PSMain(PSParticleDrawIn input) : SV_Target
{
    return float4(1,1,1,1);
}
#endif
