#ifndef CS_SKINNED_VERTEX_HLSL
#define CS_SKINNED_VERTEX_HLSL

#define MAX_BONE_TRANSFORMS 600

struct SkinnedVertex
{
    float3 Position;
    float3 Normal;
    float2 TexCoord;
    float3 Tangent;
    float4 BoneIndices;
    float4 BoneWeights;
};

struct OutVertex
{
    float3 Position;
    float3 Normal;
    float2 TexCoord;
    float3 Tangent;
    float4 Color;
};

StructuredBuffer<SkinnedVertex> g_vertices : register(t0);

cbuffer BoneBuffer : register(b0)
{
    matrix g_boneTransforms[MAX_BONE_TRANSFORMS];
}

cbuffer VertexCount: register(b1)
{
    uint g_vertexCount;
}

RWStructuredBuffer<OutVertex> g_outputVertices : register(u0);

float4x4 SkinTransform(float4 weights, float4 boneIndices)
{
    float4x4 skinTransform =
            g_boneTransforms[boneIndices.x] * weights.x +
            g_boneTransforms[boneIndices.y] * weights.y +
            g_boneTransforms[boneIndices.z] * weights.z +
            g_boneTransforms[boneIndices.w] * weights.w;
    return skinTransform;
}
void SkinVertex(inout float4 position, inout float3 normal, float4x4 skinTransform)
{
    position = mul(position, skinTransform);
    normal = mul(normal, (float3x3) skinTransform);
}

[numthreads(1024, 1, 1)]
void ComputeSkinnedVertex(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= g_vertexCount)
    {
        return;
    }

    SkinnedVertex inputVertex = g_vertices[DTid.x];
    OutVertex outputVertex;
    
    float4x4 skinTransform = SkinTransform(inputVertex.BoneWeights, inputVertex.BoneIndices);
    float4 position = float4(inputVertex.Position, 1.0f);
    SkinVertex(position, inputVertex.Normal, skinTransform);
    
    outputVertex.Position = position.xyz;
    outputVertex.Normal = inputVertex.Normal;
    outputVertex.TexCoord = inputVertex.TexCoord;
    outputVertex.Tangent = inputVertex.Tangent;
    outputVertex.Color = float4(0.f,0.f,0.f,0.f);
    g_outputVertices[DTid.x] = outputVertex;
}
#endif