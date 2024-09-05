#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;
struct ID3D12GraphicsCommandList;

/// <summary>
/// 여러개의 랜더패스를 만들기 귀찮아서 상속형태로 만들려고 함
/// 필요한 것:
/// Root Signature
/// PSO
/// 
/// </summary>
class RenderPass
{
public:
	RenderPass() {};
	virtual ~RenderPass() {};

public:
	virtual void Init() abstract;
	//virtual void Render() abstract;
	virtual void Resize() abstract;

protected:
	virtual void CreateRootSignature(ComPtr<ID3D12Device> Device) abstract;
	virtual void CreatePipelineState(ComPtr<ID3D12Device> Device) abstract;

protected:
	//ComPtr<ID3D12RootSignature> m_rootSignature;
	//ComPtr<ID3D12PipelineState> m_pipelineState;
};

