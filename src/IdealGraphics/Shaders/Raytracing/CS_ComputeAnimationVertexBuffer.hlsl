#ifndef CS_SKINNED_VERTEX_HLSL
#define CS_SKINNED_VERTEX_HLSL

#define MAX_BONE_TRANSFORMS 250

struct SkinnedVertex
{
    float3 Position;
    float3 Normal;
    float2 TexCoord;
    float4 Tangent;
    float4 BoneIndices;
    float4 BoneWeights;
};

struct OutVertex
{
    float3 Position;
    float3 Normal;
    float2 TexCoord;
    float3 Tangent;
    float padding;
};

StructuredBuffer<SkinnedVertex> g_vertices : register(t0);

cbuffer BoneBuffer : register(b0)
{
    matrix g_boneTransforms[MAX_BONE_TRANSFORMS];
}

uint g_vertexCount : register(b1);

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

[numthreads(1024, 1, 1)]
void ComputeSkinnedVertex(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= g_vertexCount)
    {
        return;
    }
    
    OutVertex vertex = g_vertices[DTid.x];
    vertex.TexCoord = g_vertices[DTid.x].TexCoord;
    vertex.Tangent = g_vertices[DTid.x].Tangent;
    vertex.Normal = g_vertices[DTid.x].Normal;
    
    float3 newPos = float3(0, 0, 0);
    for (int i = 0; i < 4; ++i)
    {
        newPos += mul(vertex.Position, (float3x3)g_boneTransforms[vertex.BoneIndices[i]]) * vertex.BoneWeights[i];
    }
    vertex.Position = newPos;
    g_outputVertices[DTid.x] = vertex;
}
#endif