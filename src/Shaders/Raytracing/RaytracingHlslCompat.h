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

    float padding[3];
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
#endif // RAYTRACINGHLSLCOMPAT_H