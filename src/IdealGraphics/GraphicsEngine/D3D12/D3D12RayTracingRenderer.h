#pragma once

#include "Core/Core.h"
#include "GraphicsEngine/public/IdealRenderer.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"

#include <d3d12.h>
#include <d3dx12.h>

#include "GraphicsEngine/D3D12/D3D12Definitions.h"
// Test
#include "GraphicsEngine/ConstantBufferInfo.h"
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

struct cbViewport
{
	float left;
	float top;
	float right;
	float bottom;
};

namespace Ideal
{
	class ResourceManager;
	class ShaderManager;

	class D3D12DescriptorHeap;
	class D3D12DynamicDescriptorHeap;
	class D3D12Texture;
	class D3D12PipelineStateObject;
	class D3D12Viewport;

	class IdealCamera;
	class IdealRenderScene;
	class IdealRaytracingRenderScene;
	class IdealStaticMeshObject;
	class IdealSkinnedMeshObject;
	class IdealScreenQuad;

	struct ConstantBufferContainer;
	// Manager
	class D3D12ConstantBufferPool;
	class D3D12DynamicConstantBufferAllocator;
	class D3D12UploadBufferPool;
	class DeferredDeleteManager;

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
	class D3D12Shader;
	class D3D12UAVBuffer;
	class D3D12ShaderResourceView;

	class DXRAccelerationStructureManager;
	class RaytracingManager;
	class D3D12DescriptorManager;
}
struct TestVertex;

namespace Ideal
{
	class D3D12RayTracingRenderer : public Ideal::IdealRenderer, public std::enable_shared_from_this<D3D12RayTracingRenderer>
	{
	public:
		static const uint32 SWAP_CHAIN_FRAME_COUNT = G_SWAP_CHAIN_NUM;
		static const uint32 MAX_PENDING_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;

	private:
		static const uint32 MAX_DRAW_COUNT_PER_FRAME = 1024;
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
		void DeleteMeshObject(std::shared_ptr<Ideal::IMeshObject> MeshObject) override;

		// 작동 안함 // 그냥 mesh object 반환
		virtual std::shared_ptr<Ideal::IMeshObject>	CreateDebugMeshObject(const std::wstring& FileName) override;

		std::shared_ptr<Ideal::IAnimation> CreateAnimation(const std::wstring& FileName) override;
		std::shared_ptr<Ideal::IRenderScene> CreateRenderScene();
		void SetRenderScene(std::shared_ptr<Ideal::IRenderScene> RenderScene);
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

		virtual void SetSkyBox(const std::wstring& FileName) override;

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

	public:
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
		std::shared_ptr<Ideal::D3D12UploadBufferPool> m_BLASInstancePool[MAX_PENDING_FRAME_COUNT] = {};

	public:
		uint64 m_lastFenceValues[MAX_PENDING_FRAME_COUNT] = {};
		uint64 m_currentContextIndex = 0;
	private:

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
		std::wstring m_assetPath;
		std::wstring m_modelPath;
		std::wstring m_texturePath;

	private:
		// Main Camera
		std::shared_ptr<Ideal::IdealCamera> m_mainCamera = nullptr;
		float m_aspectRatio = 0.f;

		// sky box
		std::shared_ptr<Ideal::D3D12Texture> m_skyBoxTexture = nullptr;

		// Manager
		std::shared_ptr<Ideal::ResourceManager> m_resourceManager = nullptr;
		std::shared_ptr<Ideal::DeferredDeleteManager> m_deferredDeleteManager = nullptr;

		// RAY TRACING FRAMEWORK
	private:
		// shader
		//std::shared_ptr<Ideal::ShaderManager> m_shaderManager;
		void CompileShader2(const std::wstring& FilePath, const std::wstring& EntryPoint, ComPtr<IDxcBlob>& OutBlob);
		ComPtr<IDxcCompiler3> m_compiler3;
		ComPtr<IDxcUtils> m_dxcUtils;
		ComPtr<IDxcBlob> m_testBlob;

		std::shared_ptr<Ideal::D3D12Shader> m_myShader;
		std::shared_ptr<Ideal::D3D12Shader> m_myShader2;
		std::shared_ptr<Ideal::D3D12Shader> m_animationShader;

	private:
		// AS
		SceneConstantBuffer m_sceneCB;

		// Render
		void CopyRaytracingOutputToBackBuffer();

		// Scene Test
		std::shared_ptr<Ideal::IdealRaytracingRenderScene> m_renderScene;
		void InitRenderScene();
		void TestDrawRenderScene();

		// AS Manager
		std::shared_ptr<Ideal::RaytracingManager> m_raytracingManager;
		std::vector<std::shared_ptr<Ideal::IdealStaticMeshObject>> m_staticMeshObject;
		std::vector<std::shared_ptr<Ideal::IdealSkinnedMeshObject>> m_skinnedMeshObject;

		// 여러 Descriptor Pool과 고정적으로 사용될 srv_cbv_uav heap을 합쳤다.
		std::shared_ptr<Ideal::D3D12DescriptorManager> m_descriptorManager;

		// 2024.07.02 Wait 뺀 버전의 BLAS , TLAS 빌드 만들기
		void RaytracingManagerInit();
		void RaytracingManagerUpdate();
		void RaytracingManagerAddObject(std::shared_ptr<Ideal::IdealStaticMeshObject> obj);
		void RaytracingManagerAddObject(std::shared_ptr<Ideal::IdealSkinnedMeshObject> obj);

		void RaytracingManagerDeleteObject(std::shared_ptr<Ideal::IdealStaticMeshObject> obj);
		void RaytracingManagerDeleteObject(std::shared_ptr<Ideal::IdealSkinnedMeshObject> obj);

		// EDITOR 
	private:
		void InitImGui();

		Ideal::D3D12DescriptorHandle m_imguiSRVHandle;
		std::shared_ptr<Ideal::D3D12DynamicDescriptorHeap> m_imguiSRVHeap;
		bool m_isEditor;

	};
}