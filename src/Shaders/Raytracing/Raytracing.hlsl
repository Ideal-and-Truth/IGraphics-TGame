//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#ifndef RAYTRACING_HLSL
#define RAYTRACING_HLSL

#define HLSL
#include "RaytracingHlslCompat.h"
#include "RaytracingHelper.hlsli"

#define SHADOW_ON

#define BeforeRefactor

#define MAX_POINT_LIGHT_NUM 16
#define MAX_SPOT_LIGHT_NUM 16

//-----------GLOBAL-----------//
RWTexture2D<float4> g_renderTarget : register(u0);
RaytracingAccelerationStructure g_scene : register(t0, space0);
TextureCube<float4> g_texEnvironmentMap : register(t1);
SamplerState LinearWrapSampler : register(s0);

ConstantBuffer<SceneConstantBuffer> g_sceneCB : register(b0);
ConstantBuffer<LightList> g_lightList : register(b1);

//-----------LOCAL-----------//
StructuredBuffer<uint> l_indices : register(t0, space1);
StructuredBuffer<PositionNormalUVTangentColor> l_vertices : register(t1, space1);
Texture2D<float4> l_texDiffuse : register(t2, space1);
Texture2D<float4> l_texNormal : register(t3, space1);
Texture2D<float4> l_texMetallic : register(t4, space1);
Texture2D<float4> l_texRoughness : register(t5, space1);
ConstantBuffer<MaterialInfoConstantBuffer> l_materialInfo : register(b0, space1);
//StructuredBuffer<MaterialInfoConstantBuffer> l_materialInfo : register(b0, space1);


typedef BuiltInTriangleIntersectionAttributes MyAttributes;

// Light



struct RayPayload
{
    unsigned int rayRecursionDepth;
    float3 radiance;
};

struct ShadowRayPayload
{
    float tHit;
};

struct SafeSpawnPointParameters
{
    float3 v0;
    float3 v1;
    float3 v2;
    float2 bary;
    float3x4 o2w;
    float3x4 w2o;
};

// Retrieve hit world position.
float3 HitWorldPosition()
{
    return WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
}

// Retrieve attribute at a hit position interpolated from vertex attributes using the hit's barycentrics.
float3 HitAttribute(float3 vertexAttribute[3], BuiltInTriangleIntersectionAttributes attr)
{
    return vertexAttribute[0] +
        attr.barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]) +
        attr.barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
}

float2 HitAttribute(float2 vertexAttribute[3], BuiltInTriangleIntersectionAttributes attr)
{
    return vertexAttribute[0] +
        attr.barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]) +
        attr.barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
}

float3 NormalMap(in float3 normal, in float2 texCoord, in PositionNormalUVTangentColor vertices[3], in MyAttributes attr)
{
    //float3 tangent;
    //float3 vertexTangents[3] =
    //{
    //    vertices[0].tangent,
    //    vertices[1].tangent,
    //    vertices[2].tangent
    //};
    //tangent = normalize(HitAttribute(vertexTangents, attr));
    //
    //float3 newNormal = l_texNormal.SampleLevel(LinearWrapSampler, texCoord, 0).xyz;
    //float3 bitangent = normalize(cross(tangent, normal));
    //newNormal = (newNormal * 2.f) - 1.f;
    //newNormal = (newNormal.x * tangent) + (newNormal.y * bitangent) + (newNormal.z * normal); //TBN변환
    //return normalize(newNormal);
    
    
    float3 tangent;
    float3 vertexTangents[3] =
    {
        vertices[0].tangent,
        vertices[1].tangent,
        vertices[2].tangent
    };
    tangent = HitAttribute(vertexTangents, attr);
    {
        //float3 v0 = vertices[0].position;
        //float3 v1 = vertices[1].position;
        //float3 v2 = vertices[2].position;
        //float2 uv0 = float2(vertices[0].uv[0],vertices[0].uv[1]);
        //float2 uv1 = float2(vertices[1].uv[0],vertices[1].uv[1]);
        //float2 uv2 = float2(vertices[2].uv[0],vertices[2].uv[1]);
        //tangent = CalculateTangent(v0, v1, v2, uv0, uv1, uv2);
    }
    float3 texSample = l_texNormal.SampleLevel(LinearWrapSampler, texCoord, 0).xyz;
    float3 bumpNormal = normalize(texSample * 2.f - 1.f);
    float3 worldNormal = BumpMapNormalToWorldSpaceNormal(bumpNormal, normal, tangent);
    return worldNormal;
}

RayPayload TraceRadianceRay(in Ray ray, in UINT currentRayRecursionDepth, float bounceContribution = 1, bool cullNonOpaque = false)
{
    RayPayload payload;
    payload.rayRecursionDepth = currentRayRecursionDepth + 1;
    payload.radiance = float3(0, 0, 0);
    
    if (currentRayRecursionDepth >= g_sceneCB.maxRadianceRayRecursionDepth)
    {
        payload.radiance = float3(133, 161, 179) / 255.0;
        //payload.radiance = float3(0, 0, 0) / 255.0;
        return payload;
    }
    
    RayDesc rayDesc;
    rayDesc.Origin = ray.origin;
    rayDesc.Direction = ray.direction;
    rayDesc.TMin = 0.001;
    rayDesc.TMax = 10000.0;
    
    UINT rayFlags = (cullNonOpaque ? RAY_FLAG_CULL_NON_OPAQUE : 0);
    rayFlags |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    
    //UINT rayFlags = 0;
    //UINT rayFlags = RAY_FLAG_CULL_NON_OPAQUE;
    TraceRay(
        g_scene,
        rayFlags,
        ~0, // instance mask
        0, // offset - path tracer radiance
        2, // Pathtacer ray type count
        0, // offset - path tracer radiance
        rayDesc,
        payload);
    return payload;
}

float3 TraceReflectedGBufferRay(in float3 hitPosition, in float3 wi, in float3 N, in float3 objectNormal, inout RayPayload rayPayload, in SafeSpawnPointParameters spawn, in float TMax = 10000)
{
    // offset 과는 관계없이 똑같은 노말값 오류 현상
    float tOffset = 0.001f;
    
    // 0
    float3 offsetAlongRay = tOffset * wi;
    float3 adjustedHitPosition = hitPosition + offsetAlongRay;
    
    // 1
    //탄젠트 오프셋(Tangent Offset):
    //float3 offsetAlongRay = tOffset * N;
    //float3 adjustedHitPosition = hitPosition + offsetAlongRay;
    
    // 2
    //float3 offsetAlongRay = tOffset * normalize(N + wi);
    //float3 adjustedHitPosition = hitPosition + tOffset * N + 1e-4 * wi;
    
    // 3
    //float3 outObjPosition, outWldPosition, outObjNormal, outWldNormal;
    //float outWldOffset;
    //nvidia::safeSpawnPoint(outObjPosition, outWldPosition, outObjNormal, outWldNormal, outWldOffset,
    //    spawn.v0,
    //    spawn.v1,
    //    spawn.v2,
    //    spawn.bary,
    //    spawn.o2w,
    //    spawn.w2o
    //);
    //float3 adjustedHitPosition = nvidia::safeSpawnPoint(hitPosition, N, outWldOffset);
    //
    //if(dot(adjustedHitPosition - hitPosition, N) < 0.f)
    //{
    //    rayPayload.radiance = float3(0, 0, 0);
    //    return rayPayload.radiance;
    //}
    
    Ray ray;
    ray.origin = adjustedHitPosition;
    ray.direction = wi;

    if(dot(ray.direction, N) <= 0)
    {
        ray.origin += N * 0.001f;
    }

    float tMin = 0;
    float tMax = TMax;

    rayPayload = TraceRadianceRay(ray, rayPayload.rayRecursionDepth, tMin, tMax);

    return rayPayload.radiance;
}

// 그림자 레이를 쏘고 geometry에 맞으면 true 반환
bool TraceShadowRayAndReportIfHit(out float tHit, in Ray ray, in UINT currentRayRecursionDepth, in bool retrieveTHit = true, in float TMax = 10000)
{
    tHit = 0; // 임시로 0으로 초기화
    if (currentRayRecursionDepth >= g_sceneCB.maxShadowRayRecursionDepth)
    {
        //return false;
        return true;
    }
    
    // 확장된 레이 세팅
    RayDesc rayDesc;
    rayDesc.Origin = ray.origin;
    rayDesc.Direction = ray.direction;
    rayDesc.TMin = 0.0;
    rayDesc.TMax = TMax;
    
    ShadowRayPayload shadowPayload = { TMax };
    
    UINT rayFlags = RAY_FLAG_CULL_NON_OPAQUE; // 투명 오브젝트 스킵
    bool acceptFirstHit = !retrieveTHit;
    if (acceptFirstHit)
    {
        // 성능 팁: 실제 적중이 필요하지 않거나 
        // 영향이 미미하거나 전혀 없는 경우 첫 번째 적중을 수락하세요.
        // 성능 향상 효과가 상당할 수 있습니다.
    
        rayFlags |= RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH;
    }
    
    // Skip closest hit shaders of tHit time is not needed.
    if (!retrieveTHit)
    {
        rayFlags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    }
    
    TraceRay(
        g_scene,
        rayFlags,
        ~0,
        1, // ray type shadow
        2, // geometry stride - path tracer count
        1, // ray type shadow
        rayDesc,
        shadowPayload);
    tHit = shadowPayload.tHit;
    
    return shadowPayload.tHit > 0;
}

bool TraceShadowRayAndReportIfHit(out float tHit, in Ray ray, in float3 N, in UINT currentRayRecursionDepth, in bool retrieveTHit = true, in float TMax = 10000)
{
    tHit = 0;
    // Only trace if the surface is facing the target.
    if (dot(ray.direction, N) > 0)
    {
        return TraceShadowRayAndReportIfHit(tHit, ray, currentRayRecursionDepth, retrieveTHit, TMax);
    }
    return false;
}

bool TraceShadowRayAndReportIfHit(in float3 hitPosition, in float3 direction, in float3 N, in RayPayload rayPayload, in float TMax = 10000)
{
    float tOffset = 0.001f;
    Ray visibilityRay = { hitPosition + tOffset * N, direction };
    float dummyTHit = 0;
    return TraceShadowRayAndReportIfHit(dummyTHit, visibilityRay, N, rayPayload.rayRecursionDepth, false, TMax);
}

void CalculateSpecularAndReflectionCoefficients(
    in float3 Albedo,
    in float metallic,
    in float roughness,
    in float3 V,
    in float3 N,
    out float3 Ks,
    out float3 Kr)
{
    // 기본 반사율 (비금속성 반사율)
    const float3 F0_non_metallic = float3(0.04, 0.04, 0.04);

    // 금속성 반사율
    float3 F0 = lerp(F0_non_metallic, Albedo, metallic);

    // Fresnel-Schlick 근사식
    float3 F = F0 + (1.0 - F0) * pow(1.0 - saturate(dot(N, V)), 5.0);
    if (metallic > 0.f)
    {
        //float3 r0 = Albedo * metallic;
        //float hv = clamp(dot(N, V), 0.0f, 1.0f);
        //float3 fresnel = r0 + (1.0f - r0) * pow(1.0f - hv, 5.0f);
        //Ks = fresnel;
        
        Ks = Albedo * metallic;
        Ks = F;
    }
    else
    {
        Ks = F0_non_metallic;
    }
    
    float roughnessFactor = pow(1.0 - roughness, 2.0); // Roughness가 높을수록 반사율을 줄임
    Kr = F * roughnessFactor;
    
    //Kr = F;
}

float3 Shade(
    inout RayPayload rayPayload,
    float2 uv,
    in float3 N,
    in float3 objectNormal,
    in float3 hitPosition,
    in SafeSpawnPointParameters spawnParameters
)
{
    float3 V = -WorldRayDirection();
    float3 L = 0;
    
    float3 Kd = l_texDiffuse.SampleLevel(LinearWrapSampler, uv, 0).xyz;
    float3 Ks;
    float3 Kr;
    const float3 Kt;
    float metallic;
    float roughness;
    if (l_materialInfo.bUseMetallicMap)
    {
        //metallic = l_texMetallic.SampleLevel(LinearWrapSampler, uv, 0).x;
        metallic = l_texMetallic.SampleLevel(LinearWrapSampler, uv, 0).x;
    }
    else
    {
        metallic = l_texMetallic.SampleLevel(LinearWrapSampler, uv, 0).x;
    }
    
    if (l_materialInfo.bUseRoughnessMap)
    {
        //roughness = l_texRoughness.SampleLevel(LinearWrapSampler, uv, 0).x;
#ifdef BeforeRefactor
        roughness = 1 - l_texRoughness.SampleLevel(LinearWrapSampler, uv, 0).a;
#else
        roughness = l_texRoughness.SampleLevel(LinearWrapSampler, uv, 0).r;
#endif
        //roughness = l_texRoughness.SampleLevel(LinearWrapSampler, uv, 0).a;
    }
    else
    {
        //roughness = l_materialInfo.roughnessFactor;
#ifdef BeforeRefactor
        roughness = 1 - l_texRoughness.SampleLevel(LinearWrapSampler, uv, 0).a;
#else
        roughness = l_texRoughness.SampleLevel(LinearWrapSampler, uv, 0).r;
#endif
        //roughness = l_texRoughness.SampleLevel(LinearWrapSampler, uv, 0).a;
    }
    //metallic = 1;
    //roughness = 0;
    //metallic = l_materialInfo.metallicFactor;
    //roughness = l_materialInfo.roughnessFactor;

    CalculateSpecularAndReflectionCoefficients(Kd, metallic, roughness, V, N, Ks, Kr);
    //Kr = 0.5f;
    //Ks = metallic;
    //roughness = 0;
  
    if (!BxDF::IsBlack(Kd) || !BxDF::IsBlack(Ks))
    {
        float3 wi = normalize(g_sceneCB.lightPosition.xyz - hitPosition);

//#ifdef SHADOW_ON
//        // Raytraced shadows
//        bool isInShadow = TraceShadowRayAndReportIfHit(hitPosition, wi, N, rayPayload);
//        L += BxDF::DirectLighting::Shade(Kd, Ks, g_sceneCB.lightDiffuseColor.xyz, isInShadow, roughness, N, V, wi);
//#else
//        L += BxDF::DirectLighting::Shade(Kd, Ks, g_sceneCB.lightDiffuseColor.xyz, false, roughness, N, V, wi);
//#endif


        int pointLightNum = g_lightList.PointLightNum;
        int spotLightNum = g_lightList.SpotLightNum;
        // Directional Light
        {
            float3 direction = normalize(g_lightList.DirLight.Direction.xyz);
            
            float3 color = g_lightList.DirLight.DiffuseColor.rgb;
            //float3 color = float3(0.3f,0.3f,0.3f);
            //float3 color = float3(0.5f,0.5f,0.5f);
            //float3 color = float3(0.f,0.f,0.f);
            bool isInShadow = TraceShadowRayAndReportIfHit(hitPosition, -direction, N, rayPayload);
            L += Ideal::Light::ComputeDirectionalLight(
            Kd,
            Ks,
            color,
            isInShadow, roughness, N, V,
            direction);
            //if(BxDF::IsBlack(L))
            //{
            //    L += 0.4f * Kd;
            //}
        }
        
        // Point Light
        {
            for (int i = 0; i < pointLightNum; ++i)
            {
                float3 position = g_lightList.PointLights[i].Position.xyz;
                float3 color = g_lightList.PointLights[i].Color.rgb;
                float range = g_lightList.PointLights[i].Range;
                
                float3 direction = normalize(position - hitPosition);
                float distance = length(position - hitPosition);
                float att = Ideal::Attenuate(distance, range);
                float intensity = g_lightList.PointLights[i].Intensity;
                
                bool isInShadow = false;
                if(distance <= range)
                {
                    isInShadow = TraceShadowRayAndReportIfHit(hitPosition, direction, N, rayPayload, distance);
                }
                float3 light = Ideal::Light::ComputePointLight
                (
                Kd,
                Ks,
                color,
                isInShadow,
                roughness,
                N,
                V,
                direction,
                distance,
                range,
                intensity
                );
                L += light;
            }
            
            for (int i = 0; i < spotLightNum; ++i)
            {
                float3 position = g_lightList.SpotLights[i].Position.xyz;
                float3 color = g_lightList.SpotLights[i].Color.rgb;
                float range = g_lightList.SpotLights[i].Range;
                float angle = g_lightList.SpotLights[i].SpotAngle;
                float intensity = g_lightList.SpotLights[i].Intensity;
                float softness = g_lightList.SpotLights[i].Softness;
                float3 direction = normalize(position - hitPosition);
                float distance = length(position - hitPosition);
                float3 lightDirection = normalize(g_lightList.SpotLights[i].Direction.xyz);
                
                bool isInShadow = false;
                if(distance <= range)
                {
                    isInShadow = TraceShadowRayAndReportIfHit(hitPosition, direction, N, rayPayload, distance);
                }
                float3 light = Ideal::Light::ComputeSpotLight2(
                     Kd,
                     Ks,
                     color,
                     isInShadow,
                     roughness,
                     N,
                     V,
                     direction,
                     distance,
                     range,
                     intensity,
                     softness,
                     lightDirection,
                     angle
                 );

                L += light;
            }
        }
    }

    
#ifdef BeforeRefactor
    // Temp : 0.2는 임시 값
    L += 0.2f * Kd;
#endif
    
    // Specular
    bool isReflective = !BxDF::IsBlack(Kr);
    //bool isReflective = Ideal::CheckReflect(Kr);
    bool isTransmissive = !BxDF::IsBlack(Kt); // 일단 굴절은 뺄까?
    
    float smallValue = 1e-6f;
    isReflective = dot(V, N) > smallValue ? isReflective : false;
    if (isReflective)
    {
        if (isReflective && (BxDF::Specular::Reflection::IsTotalInternalReflection(V, N)))
        {
            RayPayload reflectedPayLoad = rayPayload;
            float3 wi = reflect(-V, N);
            
            L += Kr * TraceReflectedGBufferRay(hitPosition, wi, N, objectNormal, reflectedPayLoad, spawnParameters);
        }
        else // No total internal reflection
        {
            float3 Fo = Ks;
            if (isReflective)
            {
                // Radiance contribution from reflection.
                float3 wi;
                //float3 Fr = Kr * BxDF::Specular::Reflection::Sample_Fr(V, wi, N, Fo);    // Calculates wi
                float3 Fr = Kr * BxDF::Specular::Reflection::Sample_Fr(V, wi, N, Fo); // Calculates wi
                RayPayload reflectedRayPayLoad = rayPayload;
                // Ref: eq 24.4, [Ray-tracing from the Ground Up]
                L += Fr * TraceReflectedGBufferRay(hitPosition, wi, N, objectNormal, reflectedRayPayLoad, spawnParameters);
                //float3 result = Fr * TraceReflectedGBufferRay(hitPosition, wi, N, objectNormal, reflectedRayPayLoad);
                //L += result;
                
            }
        }
    }
    return L;
}

// Generate a ray in world space for a camera pixel corresponding to an index from the dispatched 2D grid.
inline Ray GenerateCameraRay(uint2 index, out float3 origin, out float3 direction)
{
    float2 xy = index + 0.5f; // center in the middle of the pixel.
    float2 screenPos = xy / DispatchRaysDimensions().xy * 2.0 - 1.0;

    // Invert Y for DirectX-style coordinates.
    screenPos.y = -screenPos.y;

    // Unproject the pixel coordinate into a ray.
    float4 world = mul(float4(screenPos, 0, 1), g_sceneCB.projectionToWorld);
    
    world.xyz /= world.w;
    origin = g_sceneCB.cameraPosition.xyz;
    direction = normalize(world.xyz - origin);

    Ray ray;
    ray.origin = origin;
    ray.direction = direction;
    
    return ray;
}

// Diffuse lighting calculation.
float4 CalculateDiffuseLighting(float3 hitPosition, float3 normal)
{
    float3 pixelToLight = normalize(g_sceneCB.lightPosition.xyz - hitPosition);

    // Diffuse contribution.
    float fNDotL = max(0.0f, dot(pixelToLight, normal));

    //return g_cubeCB[1].albedo * g_sceneCB.lightDiffuseColor * fNDotL;
    return g_sceneCB.lightDiffuseColor * fNDotL;
    //float4 diffuseTextureColor = g_texDiffuse.SampleLevel(LinearWrapSampler, uv, 0);
    //return diffuseTextureColor;
}

[shader("raygeneration")]
void MyRaygenShader()
{
    float3 rayDir;
    float3 origin;
    
    // Generate a ray for a camera pixel corresponding to an index from the dispatched 2D grid.
    Ray ray = GenerateCameraRay(DispatchRaysIndex().xy, origin, rayDir);
    
    UINT currentRayRecursionDepth = 0;
    RayPayload rayPayload = TraceRadianceRay(ray, currentRayRecursionDepth);

    g_renderTarget[DispatchRaysIndex().xy] = float4(rayPayload.radiance, 1);
    return;
}

[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr)
{
    float3 hitPosition = HitWorldPosition();
    uint baseIndex = PrimitiveIndex() * 3;

    const uint3 indices = uint3(
         l_indices[baseIndex],
         l_indices[baseIndex + 1],
         l_indices[baseIndex + 2]
     );

    // Retrieve corresponding vertex normals for the triangle vertices.
    float3 vertexNormals[3] =
    {
        l_vertices[indices[0]].normal,
        l_vertices[indices[1]].normal,
        l_vertices[indices[2]].normal 
    };
    
    float3 vertexTangents[3] =
    {
        l_vertices[indices[0]].tangent,
        l_vertices[indices[1]].tangent,
        l_vertices[indices[2]].tangent
    };

    PositionNormalUVTangentColor vertexInfo[3] =
    {
        l_vertices[indices[0]],
        l_vertices[indices[1]],
        l_vertices[indices[2]]
    };

    float2 vertexTexCoords[3] = { vertexInfo[0].uv, vertexInfo[1].uv, vertexInfo[2].uv };
    float2 uv = HitAttribute(vertexTexCoords, attr);

 
    // Normal
    float3 normal;
    float3 tangent;
    float3 objectNormal;
    float3 objectTangent;
    {
        float3 vertexNormals[3] =
        {
            vertexInfo[0].normal,
            vertexInfo[1].normal,
            vertexInfo[2].normal
        };
        objectNormal = normalize(HitAttribute(vertexNormals, attr));
        float orientation = HitKind() == HIT_KIND_TRIANGLE_FRONT_FACE ? 1 : -1;
        objectNormal *= orientation;
        
        normal = normalize(mul((float3x3) ObjectToWorld3x4(), objectNormal));
    }
    //if(l_materialInfo.bUseNormalMap == false)
    if(l_materialInfo.bUseNormalMap == true)
    {
#ifdef BeforeRefactor
        normal = NormalMap(normal, uv, vertexInfo, attr);
#endif
    }
    //normal = normalize(normal);
    
    SafeSpawnPointParameters spawn;
    spawn.v0 = l_vertices[indices[0]].position;
    spawn.v1 = l_vertices[indices[1]].position;
    spawn.v2 = l_vertices[indices[2]].position;
    spawn.bary = attr.barycentrics;
    spawn.o2w = ObjectToWorld3x4();
    spawn.w2o = WorldToObject3x4();
    
    payload.radiance = Shade(payload, uv, normal, objectNormal, hitPosition, spawn);
}

[shader("closesthit")]
void MyClosestHitShader_ShadowRay(inout ShadowRayPayload rayPayload, in MyAttributes attr)
{
    rayPayload.tHit = RayTCurrent();
}

[shader("miss")]
void MyMissShader(inout RayPayload payload)
{
    float3 radiance = g_texEnvironmentMap.SampleLevel(LinearWrapSampler, WorldRayDirection(), 0).xyz;
    payload.radiance = radiance;
}

[shader("miss")]
void MyMissShader_ShadowRay(inout ShadowRayPayload rayPayload)
{
    rayPayload.tHit = 0;
}

#endif // RAYTRACING_HLSL