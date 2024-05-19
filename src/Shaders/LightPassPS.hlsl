
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
    float3 eyePos;
}

float3 DirectLight(float lightIntensity, float3 lightColor, float3 toLight, float3 normal, float3 worldPos, float3 eyePos, float3 albedo)
{
    float3 toCamera = normalize(eyePos - worldPos);
    float3 diffuse = albedo;
    float3 NdotL = max(dot(normal, toLight), 0.f);
	
    return diffuse * NdotL * lightIntensity * lightColor.rgb;
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
Texture2D gNormalTexture : register(t1);
Texture2D gPosH: register(t2);
Texture2D gPosW: register(t3);
Texture2D gDepth: register(t4);

SamplerState basicSampler : register(s0);

float4 main(VertexOut input) : SV_TARGET
{
	float4 albedo = gAlbedoTexture.Sample(basicSampler, input.uv);
	float4 normalOri = gNormalTexture.Sample(basicSampler, input.uv);
    float4 posH = gPosH.Sample(basicSampler, input.uv);
    float4 posW = gPosW.Sample(basicSampler, input.uv);
    float depth = gDepth.Sample(basicSampler, input.uv).r;
    
    if(normalOri.w >= 1.f)
    {
        return albedo;
    }
	float3 normal = normalOri.xyz;
	normalize(normal);
    
    float3 toEye = eyePos - posW.xyz;
    
    float3 lightVec = -DirLight.Direction;
    normalize(lightVec);
    
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    ambient += float4(0.f, 0.f, 0.f, 1.f) + DirLight.AmbientColor;
    float diffuseFactor = dot(lightVec, normal);
    [flatten]
    if (diffuseFactor > 0.f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.f), 0.f);
        
        diffuse += diffuseFactor * float4(0.4f, 0.4f, 0.4f, 1.f) * DirLight.DiffuseColor;
        //spec += specFactor * float4(1.f,1.f,1.f,0.f) * DirLight.

    }
    float4 litColor = albedo * (ambient + diffuse);
    litColor.a = 1.f;
    return litColor;
    
    
 //       float d = posH.z / posH.w;
    
 //   if(d <= 0.f && d >= 1.f)
 //   {  
 //       return albedo;
 //   }
    
 //   if (depth >= 1.0f)
 //   {
 //       return albedo;
 //   }
    
 //   return posH;
	//// Directional Light
 //   float3 color = DirectLight(DirLight.Intensity, DirLight.DiffuseColor.rgb, -DirLight.Direction , normal, posW.xyz, eyePos, albedo.rgb);
 //   float4 ambient = DirLight.AmbientColor;
 //   float4 finalColor = float4(color, 1.f); + ambient;
 //   return finalColor;
    //return normalOri;
}