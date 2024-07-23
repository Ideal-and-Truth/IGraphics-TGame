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

#define BEFORE_REFACTOR

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
    float3 tangent;
    float3 vertexTangents[3] = {
        vertices[0].tangent,
        vertices[1].tangent,
        vertices[2].tangent
    };
    tangent = HitAttribute(vertexTangents, attr);
    float3 texSample = l_texNormal.SampleLevel(LinearWrapSampler, texCoord, 0).xyz;
    float3 bumpNormal = normalize(texSample * 2.f - 1.f);
    float3 ret = BumpMapNormalToWorldSpaceNormal(bumpNormal, normal, tangent);
    return ret;
}

RayPayload TraceRadianceRay(in Ray ray, in UINT currentRayRecursionDepth, float bounceContribution = 1, bool cullNonOpaque = false)
{
    RayPayload payload;
    payload.rayRecursionDepth = currentRayRecursionDepth + 1;
    payload.radiance = float3(0, 0, 0);
    
    if (currentRayRecursionDepth >= g_sceneCB.maxRadianceRayRecursionDepth)
    {
        payload.radiance = float3(133, 161, 179) / 255.0;
        return payload;
    }
    
    RayDesc rayDesc;
    rayDesc.Origin = ray.origin;
    rayDesc.Direction = ray.direction;
    rayDesc.TMin = 0.001;
    rayDesc.TMax = 10000.0;
    
    UINT rayFlags = (cullNonOpaque ? RAY_FLAG_CULL_NON_OPAQUE : 0);
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

float3 TraceReflectedGBufferRay(in float3 hitPosition, in float3 wi, in float3 N, in float3 objectNormal, inout RayPayload rayPayload, in float TMax = 10000)
{
    float tOffset = 0.001f;
    float3 offsetAlongRay = tOffset * wi;
    
    float3 adjustedHitPosition = hitPosition + offsetAlongRay;
    Ray ray;
    ray.origin = adjustedHitPosition;
    ray.direction = wi;

    float tMin = 0;
    float tMax = TMax;

    rayPayload = TraceRadianceRay(ray, rayPayload.rayRecursionDepth, tMin, tMax);

    return rayPayload.radiance;
}

// 그림자 레이를 쏘고 geometry에 맞으면 true 반환
bool TraceShadowRayAndReportIfHit(out float tHit, in Ray ray, in UINT currentRayRecursionDepth, in bool retrieveTHit = true, in float TMax = 10000)
{
    tHit = 0;   // 임시로 0으로 초기화
    if(currentRayRecursionDepth >= g_sceneCB.maxShadowRayRecursionDepth)
    {
        return false;
    }
    
    // 확장된 레이 세팅
    RayDesc rayDesc;
    rayDesc.Origin = ray.origin;
    rayDesc.Direction = ray.direction;
    rayDesc.TMin = 0.0;
    rayDesc.TMax = TMax;
    
    ShadowRayPayload shadowPayload = {TMax};
    
    UINT rayFlags = RAY_FLAG_CULL_NON_OPAQUE;   // 투명 오브젝트 스킵
    bool acceptFirstHit = !retrieveTHit;
    if(acceptFirstHit)
    {
        // 성능 팁: 실제 적중이 필요하지 않거나 
        // 영향이 미미하거나 전혀 없는 경우 첫 번째 적중을 수락하세요.
        // 성능 향상 효과가 상당할 수 있습니다.
    
        rayFlags |= RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH;
    }
    
    // Skip closest hit shaders of tHit time is not needed.
    if(!retrieveTHit)
    {
        rayFlags |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    }
    
    TraceRay(
        g_scene, 
        rayFlags, 
        ~0, 
        1,  // ray type shadow
        2,  // geometry stride - path tracer count
        1,  // ray type shadow
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
    Ray visibilityRay = {hitPosition + tOffset * N, direction};
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
    // Ensure the value is within the valid range to avoid numerical issues
    float cos_theta = dot(normalize(V), normalize(N));
    cos_theta = saturate(cos_theta);

    // Calculate F0 based on whether the surface is metallic or not
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), Albedo, metallic);

    // Calculate Ks using the Fresnel function
    Ks = BxDF::Fresnel(F0, cos_theta);

    // For metallic materials, Kr is same as Ks
    if(metallic > 0.f)
    {
        Kr = Ks;
    }
    else
    {
        //float3 diffuseReflection = Albedo * (1.0f - Ks);
        //Kr = diffuseReflection * (1.f - metallic) + Ks * metallic;
        Kr = Ks * (1.0 - roughness);
    }
}

float3 Shade(
    inout RayPayload rayPayload,
    float2 uv,
    in float3 N,
    in float3 objectNormal,
    in float3 hitPosition)
{
    float3 V = -WorldRayDirection();
    float3 L = 0;
    
    const float3 Kd = l_texDiffuse.SampleLevel(LinearWrapSampler, uv, 0).xyz;
    float3 Ks;
    float3 Kr;
    const float3 Kt;
    float metallic; 

    if(!l_materialInfo.bUseMetallicMap)
    {
        metallic = l_texMetallic.SampleLevel(LinearWrapSampler, uv, 0).x;
    }
    else
    {
        metallic = l_materialInfo.metallicFactor;
    }
    
    float roughness;
    if(!l_materialInfo.bUseRoughnessMap)
    {
        roughness = l_texRoughness.SampleLevel(LinearWrapSampler, uv, 0).x;
    }
    else
    {
        roughness = l_materialInfo.roughnessFactor;
    }


    CalculateSpecularAndReflectionCoefficients(Kd, metallic, roughness, V, N, Ks, Kr);

  
    if(!BxDF::IsBlack(Kd) || !BxDF::IsBlack(Ks))
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

        // Directional Light
        {
            float3 direction = normalize(g_lightList.DirLight.Direction.xyz);
            
            float3 color = g_lightList.DirLight.DiffuseColor.rgb;
            
            bool isInShadow = TraceShadowRayAndReportIfHit(hitPosition, -direction, N, rayPayload);
            L += Ideal::Light::ComputeDirectionalLight(
            Kd, 
            Ks, 
            color, 
            isInShadow, roughness, N, V,
            direction);
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
                
                bool isInShadow = TraceShadowRayAndReportIfHit(hitPosition, direction, N, rayPayload);

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
        }
    }

    // Temp : 0.4는 임시 값
    L += 0.4f * Kd;
    
    // Specular
    bool isReflective = !BxDF::IsBlack(Kr);
    bool isTransmissive = !BxDF::IsBlack(Kt); // 일단 굴절은 뺄까?
    
    float smallValue = 1e-6f;
    isReflective = dot(V, N) > smallValue ? isReflective : false;

    //if(metallic > 0.f)
    if(isReflective)
    {
        if(isReflective && (BxDF::Specular::Reflection::IsTotalInternalReflection(V, N)))
        {
            RayPayload reflectedPayLoad = rayPayload;
            float3 wi = reflect(-V, N);
            
            L += Kr * TraceReflectedGBufferRay(hitPosition, wi, N, objectNormal, reflectedPayLoad);
        }
        else // No total internal reflection
        {
            float3 Fo = Ks;
            if (isReflective)
            {
                // Radiance contribution from reflection.
                float3 wi;
                //float3 Fr = Kr * BxDF::Specular::Reflection::Sample_Fr(V, wi, N, Fo);    // Calculates wi
                float3 Fr = BxDF::Specular::Reflection::Sample_Fr(V, wi, N, Fo);    // Calculates wi
                Fr *= Kr;
                if (any(isnan(Fr)) || any(isinf(Fr)))
                {
                    Fr = float3(0, 0, 0);
                }
                RayPayload reflectedRayPayLoad = rayPayload;
                // Ref: eq 24.4, [Ray-tracing from the Ground Up]
                //L += Fr * TraceReflectedGBufferRay(hitPosition, wi, N, objectNormal, reflectedRayPayLoad);
                float3 result = Fr * TraceReflectedGBufferRay(hitPosition, wi, N, objectNormal, reflectedRayPayLoad);
                
                L += result;
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
    float3 vertexNormals[3] = { 
        l_vertices[indices[0]].normal, 
        l_vertices[indices[1]].normal, 
        l_vertices[indices[2]].normal 
    };
    
    float3 vertexTangents[3] = {
        l_vertices[indices[0]].tangent,
        l_vertices[indices[1]].tangent,
        l_vertices[indices[2]].tangent
    };

    PositionNormalUVTangentColor vertexInfo[3] = {
        l_vertices[indices[0]],
        l_vertices[indices[1]],
        l_vertices[indices[2]]
    };

    float2 vertexTexCoords[3] = {vertexInfo[0].uv, vertexInfo[1].uv, vertexInfo[2].uv}; 
    float2 uv = HitAttribute(vertexTexCoords, attr);

 
    // Normal
    float3 normal;
    float3 tangent;
    float3 objectNormal;
    float3 objectTangent;
    {
        float3 vertexNormals[3] = {
            vertexInfo[0].normal,
            vertexInfo[1].normal,
            vertexInfo[2].normal
        };
        objectNormal = normalize(HitAttribute(vertexNormals, attr));
        float orientation = HitKind() == HIT_KIND_TRIANGLE_FRONT_FACE ? 1 : -1;
        objectNormal *= orientation;
        
        normal = normalize(mul((float3x3)ObjectToWorld3x4(), objectNormal));
    }
    //if(l_materialInfo.bUseNormalMap == false)
    if(l_materialInfo.bUseNormalMap == true)
    {
        normal = NormalMap(normal, uv, vertexInfo, attr);
    }
    payload.radiance = Shade(payload, uv, normal, objectNormal, hitPosition);
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