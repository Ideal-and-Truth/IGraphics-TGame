#pragma once

// Main Interface
#include "GraphicsEngine/public/IdealRenderer.h"

#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"

#include "GraphicsEngine/D3D12/D3D12Viewport.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/D3D12PipelineState.h"
#include "GraphicsEngine/VertexInfo.h"
//#include "GraphicsEngine/Mesh.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/Resource/Camera.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

namespace Ideal
{
	class Mesh;
	class Model;
	class MeshObject;
	class ResourceManager;

	class D3D12Texture;
	class ResourceManager;
	class D3D12PipelineStateObject;

	class IdealStaticMeshObject;
	class IdealSkinnedMeshObject;
	class IdealRenderScene;

	// Manager
	class D3D12ConstantBufferPool;

	// Interface
	class ICamera;
	class IRenderScene;
	class IMeshObject;
	class ISkinnedMeshObject;
	class IRenderScene;
}

class D3D12Renderer : public Ideal::IdealRenderer, public std::enable_shared_from_this<D3D12Renderer>
{
public:
	enum { FRAME_BUFFER_COUNT = 2 };

private:
	static const uint32 MAX_DRAW_COUNT_PER_FRAME = 256;
	static const uint32	MAX_DESCRIPTOR_COUNT = 4096;

public:
	D3D12Renderer(HWND hwnd, uint32 width, uint32 height);
	virtual ~D3D12Renderer();

public:
	//---------------------Ideal Renderer Interface-----------------------//
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;

	virtual std::shared_ptr<Ideal::ICamera> CreateCamera() override;
	virtual void SetMainCamera(std::shared_ptr<Ideal::ICamera> Camera) override;

	// Test
	virtual std::shared_ptr<Ideal::IMeshObject> CreateStaticMeshObject(const std::wstring& FileName) override;
	virtual std::shared_ptr<Ideal::ISkinnedMeshObject> CreateSkinnedMeshObject(const std::wstring& FileName) override;
	virtual std::shared_ptr<Ideal::IAnimation> CreateAnimation(const std::wstring& FileName) override;

	virtual std::shared_ptr<Ideal::IRenderScene> CreateRenderScene() override;
	virtual void SetRenderScene(std::shared_ptr<Ideal::IRenderScene> RenderScene) override;

public:
	void Release();

	void CreateDefaultCamera();
	void BeginRender();
	void EndRender();

	std::shared_ptr<Ideal::ResourceManager> GetResourceManager();

	// Command
	void CreateCommandList();

	// Fence
	void CreateGraphicsFence();
	uint64 GraphicsFence();
	void WaitForGraphicsFenceValue();

	void GraphicsPresent();

public:
	ComPtr<ID3D12Device> GetDevice();
	// 일단 cmd list는 하나만 쓴다.
	ComPtr<ID3D12GraphicsCommandList> GetCommandList();
	//Ideal::D3D12PipelineState
	uint32 GetFrameIndex() const;

	//Matrix GetView() { return m_view; }
	//Matrix GetProj() { return m_proj; }
	//Matrix GetViewProj() { return m_viewProj; }
	Matrix GetView() { return m_mainCamera->GetView(); }
	Matrix GetProj() { return m_mainCamera->GetProj(); }
	Matrix GetViewProj() { return m_mainCamera->GetViewProj(); }

	// 2024.04.21 Temp : Ideal::DescriptorHeap
	Ideal::D3D12DescriptorHandle AllocateSRV() { return m_idealSrvHeap.Allocate(); }
	uint32 m_srvHeapNum = 256U;
	Ideal::D3D12DescriptorHeap m_idealSrvHeap;

private:
	uint32 m_width;
	uint32 m_height;
	HWND m_hwnd;

	// Device
	ComPtr<ID3D12Device> m_device = nullptr;
	ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
	ComPtr<IDXGISwapChain3> m_swapChain = nullptr;
	uint32 m_frameIndex;

	// RTV
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap = nullptr;
	uint32 m_rtvDescriptorSize;
	ComPtr<ID3D12Resource> m_renderTargets[FRAME_BUFFER_COUNT];

	// DSV
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap = nullptr;
	ComPtr<ID3D12Resource> m_depthStencil = nullptr;

	// Command
	ComPtr<ID3D12CommandAllocator> m_commandAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> m_commandList = nullptr;

	// Fence
	ComPtr<ID3D12Fence> m_graphicsFence = nullptr;
	uint64 m_graphicsFenceValue = 0;
	HANDLE m_graphicsFenceEvent = NULL;

	// RenderScene
	std::shared_ptr<Ideal::IdealRenderScene> m_currentRenderScene;

public:
	// Test : 2024.05.07; 하나의 Descriptor Table에서 떼어다가 쓴다...
	void CreateDescriptorTable();
	std::shared_ptr<Ideal::D3D12DescriptorHeap> GetMainDescriptorHeap();

	// Test : 2024.05.07; Type마다 CBPool을 만들어야하나?
	void CreateCBPool();
	// 2024.05.08; 256의 512, 1024 로 만들어서 필요한 용량에 따라 사용해도 괜찮을 듯?
	std::shared_ptr<Ideal::D3D12ConstantBufferPool> GetCBPool(uint32 SizePerCB);

private:
	// D3D12 Data Manager
	std::shared_ptr<Ideal::D3D12DescriptorHeap> m_descriptorHeap;

	// 2024.05.08; 256의 512, 1024 로 만들어서 필요한 용량에 따라 사용해도 괜찮을 듯?
	std::shared_ptr<Ideal::D3D12ConstantBufferPool> m_cb256Pool;
	std::shared_ptr<Ideal::D3D12ConstantBufferPool> m_cb512Pool;
	std::shared_ptr<Ideal::D3D12ConstantBufferPool> m_cb1024Pool;

private:
	float m_aspectRatio = 0.f;

private:
	const float m_offsetSpeed = 0.02f;

private:
	Ideal::D3D12Viewport m_viewport;

private:
	// Resource Manager
	std::shared_ptr<Ideal::ResourceManager> m_resourceManager = nullptr;

	// Main Camera
	std::shared_ptr<Ideal::Camera> m_mainCamera = nullptr;

private:
	// Test Camera 2
	std::shared_ptr<Ideal::ICamera> c1;
	std::shared_ptr<Ideal::ICamera> c2;

public:
	//--------Asset Info---------//
	virtual void SetAssetPath(const std::wstring& AssetPath) override { m_assetPath = AssetPath; }
	virtual void SetModelPath(const std::wstring& ModelPath) override { m_modelPath = ModelPath; }
	virtual void SetTexturePath(const std::wstring& TexturePath) override { m_texturePath = TexturePath; }

	virtual void ConvertAssetToMyFormat(std::wstring FileName, bool isSkinnedData = false) override;
	virtual void ConvertAnimationAssetToMyFormat(std::wstring FileName) override;

private:
	std::wstring m_assetPath;
	std::wstring m_modelPath;
	std::wstring m_texturePath;
};