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

#ifndef RAYTRACINGHLSLCOMPAT_H
#define RAYTRACINGHLSLCOMPAT_H

#ifdef HLSL
#include "HlslCompat.h"
#else
using namespace DirectX;

// Shader will use byte encoding to access indices.
typedef UINT32 Index;
#endif

struct Ray
{
	XMFLOAT3 origin;
	XMFLOAT3 direction;
};

struct SceneConstantBuffer
{
    XMMATRIX projectionToWorld;
    XMVECTOR cameraPosition;
    XMVECTOR lightPosition;
    XMVECTOR lightAmbientColor;
    XMVECTOR lightDiffuseColor;

    unsigned int maxRadianceRayRecursionDepth;
    unsigned int maxShadowRayRecursionDepth;

    XMVECTOR Color;
    float padding[2];
};

struct CubeConstantBuffer
{
    XMFLOAT4 albedo;
};

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
};

struct PositionNormalUVVertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    float uv[2];
};

struct PositionNormalUVTangentColor
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    float uv[2];
    XMFLOAT3 tangent;
    XMFLOAT4 color;

};

struct DirectionalLight
{
    XMFLOAT4 AmbientColor;
    XMFLOAT4 DiffuseColor;
    XMFLOAT3 Direction;
    float Intensity;
};

struct PointLight
{
    XMFLOAT4 Color;
    XMFLOAT3 Position;
    float Range;
    float Intensity;
    float pad0;
    float pad1;
    float pad2;
};

struct SpotLight
{
    XMFLOAT4 Color;
    XMFLOAT4 Direction;
    XMFLOAT3 Position;
    float SpotAngle;
    float Range;
    float Intensity;
    float pad0;
    float pad1;
};

struct LightList
{
    int PointLightNum;
    int SpotLightNum;
	float pad;
	float pad1;

    DirectionalLight DirLight;
    PointLight PointLights[16];  //TEMP
    SpotLight SpotLights[16];    //TEMP


};
#endif // RAYTRACINGHLSLCOMPAT_H