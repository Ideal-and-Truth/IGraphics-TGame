#ifndef DEFAULT_PARTICLE_BILLBOARD_VS_HLSL
#define DEFAULT_PARTICLE_BILLBOARD_VS_HLSL

#include "ParticleCommon.hlsli" // necessary

VSParticleDrawOut VSMain(VSParticleInput input)
{
    VSParticleDrawOut output;
    //output.Pos = float3(0,0,0);
    //output.Pos = float3(input.ID * 10,0,0);
    output.Pos = g_bufPos[input.ID].Position.xyz;
    output.Color = float4(1,1,1,1);
    return output;
}

[maxvertexcount(4)]
void GSMain(point VSParticleDrawOut input[1], inout TriangleStream<VSOutput> SpriteStream)
{
    VSOutput output;
   

    // 빌보드를 만들겠다
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = eyePos - input[0].Pos;
    //look.y = 0.0f;  // y 축 정렬이므로 xz 평면에 투영
    look = normalize(look);
    float3 right = cross(up, look);
    up = cross(look, right);
    up = normalize(up);
    right = normalize(right);
    float halfWidth = 0.5f; // * input[0].Size.x // TODO : 나중에 사이즈 추가 할 것
    float halfHeight = 0.5f; // * input[0].Size.y // TODO : 나중에 사이즈 추가 할 것
    float4 v[4];
    v[0] = float4(input[0].Pos + halfWidth * right - halfHeight * up, 1.f);
    v[1] = float4(input[0].Pos + halfWidth * right + halfHeight * up, 1.f);
    v[2] = float4(input[0].Pos - halfWidth * right - halfHeight * up, 1.f);
    v[3] = float4(input[0].Pos - halfWidth * right + halfHeight * up, 1.f);
    
    for(int i = 0; i < 4; i++)
    {
        //float3 position = g_positions[i];
        //position = mul(position, (float3x3)WorldInvTranspose) + input[0].Pos;
        float3 position = v[i].xyz;
        position = mul(position, (float3x3)WorldInvTranspose);// + input[0].Pos;
        output.PosH = mul(float4(position, 1.0), mul(World, ViewProj));
        output.Pos = position;
        //output.Color = input[0].Color;
        // TEMP : 노말값 대충
        output.Normal = float3(1.f,0.f,0.f);
        output.UV = g_texcoords[i];
        SpriteStream.Append(output);
    }
    SpriteStream.RestartStrip();    // 삼각형 목록을 초기화
}

// TEST
float4 PSMain(VSOutput input) : SV_Target
{
    float4 ret;
    ret = ParticleTexture0.Sample(LinearWrapSampler, input.UV);
    return ret;
}

#endif
