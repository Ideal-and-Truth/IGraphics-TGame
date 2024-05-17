
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

#define MAX_POINT_LIGHT_NUM 16
#define MAX_SPOT_LIGHT_NUM 16

struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
	float Intensity;
};

struct PointLight
{
	float4 Color;
	float3 Position;
	float Range;
	float Intensity;
};

struct SpotLight
{
	float4 Color;
	float4 Direction;
	float3 Position;
	float SpotAngle;
	float Range;
	float Intensity;
};

cbuffer LightList : register(b1)
{
    DirectionalLight DirLight;
	PointLight PointLights[MAX_POINT_LIGHT_NUM];
	SpotLight SpotLights[MAX_SPOT_LIGHT_NUM];
};

// cbuffer LightList : register(b1)
// {
//     
// }

Texture2D gAlbedoTexture : register(t0);
Texture2D gTexture2 : register(t1);
Texture2D gNormalTexture : register(t2);

SamplerState basicSampler : register(s0);

float4 main(VertexOut input) : SV_TARGET
{
    float4 albedo = gAlbedoTexture.Sample(basicSampler, input.uv);
    float4 color2 = gNormalTexture.Sample(basicSampler, input.uv);
    float4 color3 = gTexture2.Sample(basicSampler, input.uv);
    //color += color2;
    //color += color3;

	float4 normalOri = gNormalTexture.Sample(basicSampler, input.uv);
	float3 normal = normalOri.xyz;
	normalize(normal);

	// Dir Test
	//float4 ambient = DirLight.AmbientColor;
	//float4 diffuse = DirLight.DiffuseColor;
	float3 lightVec =  -DirLight.Direction;
	//float diffuseFactor = dot(lightVec, normal);
	float diffuseFactor = dot(normal, lightVec);
	float4 matDiffuse = float4(0.4f,0.4f,0.4f,0.4f);
	float4 diffuse = diffuseFactor * matDiffuse * DirLight.DiffuseColor;
	float4 ambient = DirLight.AmbientColor;

	float4 FinalColor = albedo * (ambient + diffuse);
	FinalColor.a = 1.f;

	return FinalColor;
// 	float4 dirLight = float4(DirLight.Direction, 1.f);
// 	float4 value = dot(-dirLight, normalize(gNormalTexture.Sample(basicSampler, input.uv)));
// 	float4 finalColor = color * value;
//     return finalColor;

	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}