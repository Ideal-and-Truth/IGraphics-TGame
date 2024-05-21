#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"

struct VertexTest2
{
	Vector3 Position;
	Vector2 UV;
};

struct BasicVertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector2 UV;
	Vector3 Tangent;
	Vector4 Color;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

public:
	static const int32 InputElementCount = 5;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct SkinnedVertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector2 UV;
	Vector3 Tangent;
	Vector4 BlendIndices;
	Vector4 BlendWeights;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

public:
	static const int32 InputElementCount = 6;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct alignas(256) SimpleBoxConstantBuffer
{
	Matrix worldViewProjection;
};

struct Mesh
{
	std::vector<BasicVertex> vertices;
	std::vector<uint32> indices;
	std::wstring diffuseMap;
};

struct ScreenQuadVertex
{
	Vector3 Position;
	Vector2 UV;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

public:
	static const int32 InputElementCount = 2;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};