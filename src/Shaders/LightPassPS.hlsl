
struct VertexOut
{
	float4 PosH : SV_Position;
    float2 uv : TEXCOORD;
};

cbuffer Global : register(b0)
{
    float4x4 View;
    float4x4 Proj;
    float4x4 ViewProj;
}

Texture2D gTexture0 : register(t0);
Texture2D gTexture1 : register(t1);
Texture2D gTexture2 : register(t2);

SamplerState basicSampler : register(s0);

float4 main(VertexOut input) : SV_TARGET
{
    float4 color = gTexture0.Sample(basicSampler, input.uv);
    float4 color2 = gTexture1.Sample(basicSampler, input.uv);
    float4 color3 = gTexture2.Sample(basicSampler, input.uv);
    //color += color2;
    //color += color3;
    return color;

	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}