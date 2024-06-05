#pragma once

#include "Core/Core.h"
#include "GraphicsEngine/public/IdealRenderer.h"

#include <d3d12.h>
//#include "d3dx12.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
//Test
#include "GraphicsEngine/VertexInfo.h"

#include <dxcapi.h>
#include <d3dx12.h>
#include <atlbase.h>

struct ID3D12Device;
struct ID3D12CommandQueue;
struct IDXGISwapChain3;
struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

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

	private:
		void InitializeBLAS();
		void InitializeTLAS();
		void UploadBuffer(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info, ComPtr<ID3D12Resource> Scratch, ComPtr<ID3D12Resource> Result);
		void CreateAS(D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS input, ComPtr<ID3D12Resource> Scratch, ComPtr<ID3D12Resource> Result);
		void InitShader();

		std::shared_ptr<Ideal::IdealMesh<TestVertex>> mesh;
		ComPtr<ID3D12Resource> blasScratch;
		ComPtr<ID3D12Resource> blasResult;
		ComPtr<ID3D12Resource> tlasScratch;
		ComPtr<ID3D12Resource> tlasResult;
		
		CComPtr<IDxcIncludeHandler> dxcIncIncludeHandler;

		// 컴파일된 셰이더 DXIL 바이트 코드
		ComPtr<IDxcBlob> rgsBytecode;
		ComPtr<IDxcBlob> missBytecode;
		ComPtr<IDxcBlob> chsBytecode;
		ComPtr<IDxcBlob> ahsBytecode;

		// 레이 트레이싱 파이프라인 상태 오브젝트
		void CreateRTPSO();
		D3D12_STATE_SUBOBJECT CreateShaderSubobject(const wchar_t* Name, const wchar_t* EntryName, ComPtr<IDxcBlob> ShaderByteCode);

		ComPtr<ID3D12StateObject> rtpso;
		ComPtr<ID3D12StateObjectProperties> rtpsoInfo;

		// 셰이더, 루트 서명 ,설정 데이터에 대한 상태 서브오브젝트 정의
		std::vector<D3D12_STATE_SUBOBJECT> subobjects;

		void InitShaderSubobject();
		void InitHitGroupSubobject();
	};
}