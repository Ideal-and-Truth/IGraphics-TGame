// bone �����Ű�� ���� simple Mesh Shaderr

#define MAX_BONE_TRANSFORMS 50

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 tangent : TANGENT;
	uint4  skinIndices	: BLENDINDICES;
	float4 skinWeights	: BLENDWEIGHT;
};

struct VSOutput
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
    float4 NormalW : NORMAL;
    float2 UV : TEXCOORD;
    uint4  skinIndices	: BLENDINDICES;
    float4 skinWeights	: BLENDWEIGHT;
};

cbuffer Transform : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
    float4x4 WorldInvTranspose;
}

cbuffer BoneBuffer : register(b1)
{
    matrix BoneTransforms[MAX_BONE_TRANSFORMS];
}

//uint BoneIndex;

Texture2D diffuseTexture : register(t0);
SamplerState sampler0 : register(s0);

VSOutput VS(VSInput input)
{
    VSOutput output;

    float4 localPos = float4(input.Pos, 1.f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    output.PosW = worldPos;
    output.PosH = projPos;
    output.NormalW = mul(WorldInvTranspose, float4(input.Normal, 0.f));
    output.UV = input.UV;
    output.skinIndices = input.skinIndices;
    output.skinWeights = input.skinWeights;
    return output;
}

float4 PS(VSOutput input) : SV_TARGET
{
    //float4 color = diffuseTexture.Sample(sampler0, input.UV);
    float4 color = input.skinWeights;
    //float4 color = float4(input.UV.xy, 1, 1);
    return color;
}