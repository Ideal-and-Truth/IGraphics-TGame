#pragma once

#include "Core/Core.h"
#include "GraphicsEngine/public/IdealRenderer.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"

#include <d3d12.h>
#include <d3dx12.h>

#include "GraphicsEngine/D3D12/D3D12Definitions.h"
//Test
#include "GraphicsEngine/VertexInfo.h"
#include <dxgi.h>
#include <dxgi1_6.h>

#include <dxcapi.h>
#include <atlbase.h>

struct ID3D12Device;
struct ID3D12CommandQueue;
struct IDXGISwapChain3;
struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

namespace GlobalRootSignatureParams {
	enum Value {
		OutputViewSlot = 0,
		AccelerationStructureSlot,
		Count
	};
}

namespace LocalRootSignatureParams {
	enum Value {
		ViewportConstantSlot = 0,
		Count
	};
}

struct cbViewport
{
	float left;
	float top;
	float right;
	float bottom;
};

struct RayGenConstantBuffer
{
	cbViewport viewport;
	cbViewport stencil;
};

namespace Ideal
{
	class ResourceManager;

	class D3D12DescriptorHeap;
	class D3D12DynamicDescriptorHeap;
	class D3D12Texture;
	class D3D12PipelineStateObject;
	class D3D12Viewport;

	class IdealCamera;
	class IdealRenderScene;
	class IdealStaticMeshObject;
	class IdealSkinnedMeshObject;
	class IdealScreenQuad;

	struct ConstantBufferContainer;
	// Manager
	class D3D12ConstantBufferPool;
	class D3D12DynamicConstantBufferAllocator;

	// Interface
	class ICamera;
	class IRenderScene;
	class IMeshObject;
	class ISkinnedMeshObject;
	class IRenderScene;


	// TEST : DELETE
	template<typename>
	class IdealMesh;
	class D3D12VertexBuffer;
	class D3D12IndexBuffer;
}
struct TestVertex;

namespace Ideal
{
	class D3D12RayTracingRenderer : public Ideal::IdealRenderer, public std::enable_shared_from_this<D3D12RayTracingRenderer>
	{
	public:
		static const uint32 SWAP_CHAIN_FRAME_COUNT = SWAP_CHAIN_NUM;
		static const uint32 MAX_PENDING_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;

	private:
		static const uint32 MAX_DRAW_COUNT_PER_FRAME = 256;
		static const uint32	MAX_DESCRIPTOR_COUNT = 4096;

	public:
		D3D12RayTracingRenderer(HWND hwnd, uint32 Width, uint32 Height, bool EditorMode);
		virtual ~D3D12RayTracingRenderer();

	public:
		void Init() override;
		void Tick() override;
		void Render() override;
		void Resize(UINT Width, UINT Height) override;

		std::shared_ptr<ICamera> CreateCamera() override;
		void SetMainCamera(std::shared_ptr<ICamera> Camera) override;
		std::shared_ptr<Ideal::IMeshObject> CreateStaticMeshObject(const std::wstring& FileName) override;
		std::shared_ptr<Ideal::ISkinnedMeshObject> CreateSkinnedMeshObject(const std::wstring& FileName) override;
		std::shared_ptr<Ideal::IAnimation> CreateAnimation(const std::wstring& FileName) override;
		std::shared_ptr<Ideal::IRenderScene> CreateRenderScene() override;
		void SetRenderScene(std::shared_ptr<Ideal::IRenderScene> RenderScene) override;
		std::shared_ptr<Ideal::IDirectionalLight> CreateDirectionalLight() override;
		std::shared_ptr<Ideal::ISpotLight> CreateSpotLight() override;
		std::shared_ptr<Ideal::IPointLight> CreatePointLight() override;
		void SetAssetPath(const std::wstring& AssetPath) override;
		void SetModelPath(const std::wstring& ModelPath) override;
		void SetTexturePath(const std::wstring& TexturePath) override;
		void ConvertAssetToMyFormat(std::wstring FileName, bool isSkinnedData = false, bool NeedVertexInfo = false) override;
		void ConvertAnimationAssetToMyFormat(std::wstring FileName) override;
		bool SetImGuiWin32WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
		void ClearImGui() override;

	private:
		void CreateCommandlists();
		void CreatePools();
		void CreateSwapChains(ComPtr<IDXGIFactory6> Factory);
		void CreateRTV();
		void CreateDSVHeap();
		void CreateDSV(uint32 Width, uint32 Height);
		void CreateFence();

	private:
		void CreateDefaultCamera();

	private:
		void ResetCommandList();
		void BeginRender();
		void EndRender();
		void Present();

		uint64 Fence();
		void WaitForFenceValue(uint64 ExpectedFenceValue);

	private:
		uint32 m_width = 0;
		uint32 m_height = 0;
		HWND m_hwnd;

		std::shared_ptr<Ideal::D3D12Viewport> m_viewport = nullptr;

		ComPtr<ID3D12Device5> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;

		ComPtr<ID3D12GraphicsCommandList4> m_commandLists[MAX_PENDING_FRAME_COUNT];
		ComPtr<ID3D12CommandAllocator> m_commandAllocators[MAX_PENDING_FRAME_COUNT];
		std::shared_ptr<Ideal::D3D12DescriptorHeap> m_descriptorHeaps[MAX_PENDING_FRAME_COUNT] = {};
		std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> m_cbAllocator[MAX_PENDING_FRAME_COUNT] = {};
		uint64 m_lastFenceValues[MAX_PENDING_FRAME_COUNT] = {};
		uint64 m_currentContextIndex = 0;

		ComPtr<IDXGISwapChain3> m_swapChain = nullptr;
		UINT m_swapChainFlags;
		uint32 m_frameIndex = 0;
		// RTV
		std::shared_ptr<Ideal::D3D12DescriptorHeap> m_rtvHeap;
		std::shared_ptr<Ideal::D3D12Texture> m_renderTargets[SWAP_CHAIN_FRAME_COUNT];
		// DSV
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap = nullptr;
		ComPtr<ID3D12Resource> m_depthStencil = nullptr;

		ComPtr<ID3D12Fence> m_fence = nullptr;
		uint64 m_fenceValue = 0;
		HANDLE m_fenceEvent = NULL;

	private:
		// Main Camera
		std::shared_ptr<Ideal::IdealCamera> m_mainCamera = nullptr;
		float m_aspectRatio = 0.f;

		// resource Manager
		std::shared_ptr<Ideal::ResourceManager> m_resourceManager = nullptr;


		// RAY TRACING FRAMEWORK
	private:
		// shader
		void CreateShaderCompiler();
		void CompileShader(IDxcLibrary* lib, IDxcCompiler* comp, LPCWSTR filename, IDxcBlob** blob);

		ComPtr<IDxcCompiler> m_compiler;
		ComPtr<IDxcLibrary> m_library;
		CComPtr<IDxcIncludeHandler> dxcIncIncludeHandler;

	private:

		// 다시 //
		const wchar_t* m_raygenShaderName = L"MyRaygenShader";
		const wchar_t* m_closestHitShaderName = L"MyClosestHitShader";
		const wchar_t* m_missShaderName = L"MyMissShader";
		const wchar_t* m_hitGroupName = L"MyHitGroup";

		void CreateDeviceDependentResources();
		void CreateRayTracingInterfaces();
		void CreateRootSignatures();
		void CreateRootSignatures2();

		void SerializeAndCreateRayTracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootsig);
		ComPtr<ID3D12RootSignature> m_raytracingGlobalRootSignature;
		ComPtr<ID3D12RootSignature> m_raytracingLocalRootSignature;
		RayGenConstantBuffer m_rayGenCB;
		void CreateRaytracingPipelineStateObject();
		void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
		ComPtr<ID3D12StateObject> m_dxrStateObject;

		void BuildGeometry();
		void BuildAccelerationStructures();
		
		void BuildShaderTables();
		
		// 레이트레이싱을 위한 2d output texture를 만든다.
		void CreateRayTracingOutputResources();
		ComPtr<ID3D12Resource> m_raytracingOutput;
		uint32 m_raytracingOutputResourceUAVDescriptorHeapIndex;
		void CreateDescriptorHeap();
		std::shared_ptr<Ideal::D3D12DynamicDescriptorHeap> m_uavDescriptorHeap;
		Ideal::D3D12DescriptorHandle m_raytacingOutputResourceUAVGpuDescriptorHandle;

		//geometry
		std::shared_ptr<Ideal::D3D12VertexBuffer> m_vertexBuffer;
		std::shared_ptr<Ideal::D3D12IndexBuffer> m_indexBuffer;
		ComPtr<ID3D12Resource> m_vb2;
		ComPtr<ID3D12Resource> m_ib2;


		// AS
		ComPtr<ID3D12Resource> m_accelerationStructure;
		ComPtr<ID3D12Resource> m_bottomLevelAccelerationStructure;
		ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;

		RayGenConstantBuffer m_cbRayGen;

		ComPtr<ID3D12Resource> m_missShaderTable;
		ComPtr<ID3D12Resource> m_hitGroupShaderTable;
		ComPtr<ID3D12Resource> m_rayGenShaderTable;


		// Render
		void DoRaytracing();
		void CopyRaytracingOutputToBackBuffer();

		void UpdateForSizeChange(uint32 Width, uint32 Height);
	};
}