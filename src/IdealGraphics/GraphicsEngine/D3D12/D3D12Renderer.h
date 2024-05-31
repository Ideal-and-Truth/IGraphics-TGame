#pragma once

// Main Interface
#include "Core/Core.h"
#include "GraphicsEngine/public/IdealRenderer.h"


#include <d3d12.h>
#include "d3dx12.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
//#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
//#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
// TEMP
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

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
}

namespace Ideal
{
	class D3D12Renderer : public Ideal::IdealRenderer, public std::enable_shared_from_this<D3D12Renderer>
	{
	public:
		enum { FRAME_BUFFER_COUNT = 2 };
		static const uint32 SWAP_CHAIN_FRAME_COUNT = 2;	// TEMP : 사실 FRAME_BUFFER_COUNT와 동일하다.
		static const uint32 MAX_PENDING_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;

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

		virtual std::shared_ptr<Ideal::IMeshObject> CreateStaticMeshObject(const std::wstring& FileName) override;
		virtual std::shared_ptr<Ideal::ISkinnedMeshObject> CreateSkinnedMeshObject(const std::wstring& FileName) override;
		virtual std::shared_ptr<Ideal::IAnimation> CreateAnimation(const std::wstring& FileName) override;

		virtual std::shared_ptr<Ideal::IRenderScene> CreateRenderScene() override;
		virtual void SetRenderScene(std::shared_ptr<Ideal::IRenderScene> RenderScene) override;

		virtual std::shared_ptr<Ideal::IDirectionalLight>	CreateDirectionalLight() override;
		virtual std::shared_ptr<Ideal::ISpotLight>			CreateSpotLight() override;
		virtual std::shared_ptr<Ideal::IPointLight>			CreatePointLight() override;

		//--------Asset Info---------//
		virtual void SetAssetPath(const std::wstring& AssetPath) override { m_assetPath = AssetPath; }
		virtual void SetModelPath(const std::wstring& ModelPath) override { m_modelPath = ModelPath; }
		virtual void SetTexturePath(const std::wstring& TexturePath) override { m_texturePath = TexturePath; }

		virtual void ConvertAssetToMyFormat(std::wstring FileName, bool isSkinnedData = false, bool NeedVertexInfo = false) override;
		virtual void ConvertAnimationAssetToMyFormat(std::wstring FileName) override;

		//--------ImGui--------//
		virtual bool SetImGuiWin32WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
		virtual void ClearImGui() override;

	public:
		void Release();

		//-------Device------//
		ComPtr<ID3D12Device> GetDevice();

		//------Render-----//
		void ResetCommandList();
		void BeginRender();
		void EndRender();
		void GraphicsPresent();
		uint32 GetFrameIndex() const;

		//------CommandList------//
		void CreateCommandList();
		ComPtr<ID3D12GraphicsCommandList> GetCommandList();
		ComPtr<ID3D12CommandQueue> GetCommandQueue() { return m_commandQueue; }
		//------Fence------//
		void CreateGraphicsFence();
		uint64 GraphicsFence();
		void WaitForGraphicsFenceValue();

		//------Graphics Manager------//
		std::shared_ptr<Ideal::ResourceManager> GetResourceManager();

		// Test : 2024.05.07; 하나의 Descriptor Table에서 떼어다가 쓴다...
		// 2024.05.08 : 메인 Descriptor Heap
		void CreateDescriptorHeap();
		std::shared_ptr<Ideal::D3D12DescriptorHeap> GetMainDescriptorHeap();

		// 2024.05.31 : Dynamic ConstantBuffer Allocator 에서 직접 CBContainer를 받아오도록 수정
		std::shared_ptr<Ideal::ConstantBufferContainer> ConstantBufferAllocate(uint32 SizePerCB);
		

		//------Camera------//
		void CreateDefaultCamera(); // default camera
		Matrix GetView();
		Matrix GetProj();
		Matrix GetViewProj();
		Vector3 GetEyePos();

		//-----etc-----//
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() { return m_dsvHeap->GetCPUDescriptorHandleForHeapStart(); }
		std::shared_ptr<Ideal::D3D12Viewport> GetViewport() { return m_viewport; }

		//----Screen----//
		uint32 GetWidth() { return m_width; }
		uint32 GetHeight() { return m_height; }

	private:
		uint32 m_width = 0;
		uint32 m_height = 0;
		HWND m_hwnd;

		// Device
		ComPtr<ID3D12Device> m_device = nullptr;
		ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
		ComPtr<IDXGISwapChain3> m_swapChain = nullptr;
		uint32 m_frameIndex = 0;

		// RTV
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap = nullptr;
		uint32 m_rtvDescriptorSize = 0;
		ComPtr<ID3D12Resource> m_renderTargets[FRAME_BUFFER_COUNT];


		// DSV
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap = nullptr;
		ComPtr<ID3D12Resource> m_depthStencil = nullptr;

		// Command
		ComPtr<ID3D12CommandAllocator> m_commandAllocator = nullptr;
		ComPtr<ID3D12GraphicsCommandList> m_commandList = nullptr;

		// Fence
		ComPtr<ID3D12Fence> m_fence = nullptr;
		uint64 m_fenceValue = 0;
		HANDLE m_fenceEvent = NULL;

		// RenderScene
		std::shared_ptr<Ideal::IdealRenderScene> m_currentRenderScene;

	private:
		// D3D12 Data Manager
		std::shared_ptr<Ideal::D3D12DescriptorHeap> m_descriptorHeap = nullptr;

	private:
		float m_aspectRatio = 0.f;

	private:
		//Ideal::D3D12Viewport m_viewport;
		std::shared_ptr<Ideal::D3D12Viewport> m_viewport = nullptr;

	private:
		// Resource Manager
		std::shared_ptr<Ideal::ResourceManager> m_resourceManager = nullptr;

		// Main Camera
		std::shared_ptr<Ideal::IdealCamera> m_mainCamera = nullptr;

	private:
		// Test Camera 2
		std::shared_ptr<Ideal::ICamera> c1 = nullptr;
		std::shared_ptr<Ideal::ICamera> c2 = nullptr;

	private:
		std::wstring m_assetPath;
		std::wstring m_modelPath;
		std::wstring m_texturePath;

		std::shared_ptr<Ideal::D3D12Texture> t1 = nullptr;

		// EDITOR TEST
	private:
		void InitImgui();
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		Ideal::D3D12DescriptorHandle m_imguiSRVHandle;

		// Warning Off
	private:
		// debuging messeage OFF
		void OffWarningRenderTargetClearValue();

		// Overlapped Redering
		// D3D12 Frame Resources
		// 2024.05.28
	private:
		ComPtr<ID3D12CommandAllocator> m_commandAllocators[MAX_PENDING_FRAME_COUNT];
		ComPtr<ID3D12GraphicsCommandList> m_commandLists[MAX_PENDING_FRAME_COUNT];
		std::shared_ptr<Ideal::D3D12DescriptorHeap> m_descriptorHeaps[MAX_PENDING_FRAME_COUNT];
		std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> m_cbAllocator[MAX_PENDING_FRAME_COUNT];
		uint64 m_lastFenceValues[MAX_PENDING_FRAME_COUNT];
		uint64 m_currentContextIndex = 0;

		void CreateAndInitOverlappedRenderingResources();
		uint64 OverlappedRenderingFence();
		void OverlappedBeginRender();
		void OverlappedEndRender();
		void OverlappedRenderingPresent();
		void WaitForFenceValue(uint64 ExpectedFenceValue);
	};
}