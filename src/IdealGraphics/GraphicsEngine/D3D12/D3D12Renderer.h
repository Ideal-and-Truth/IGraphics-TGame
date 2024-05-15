#pragma once

// Main Interface
#include "Core/Core.h"
#include "GraphicsEngine/public/IdealRenderer.h"

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

	// Manager
	class D3D12ConstantBufferPool;

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

		//--------Asset Info---------//
		virtual void SetAssetPath(const std::wstring& AssetPath) override { m_assetPath = AssetPath; }
		virtual void SetModelPath(const std::wstring& ModelPath) override { m_modelPath = ModelPath; }
		virtual void SetTexturePath(const std::wstring& TexturePath) override { m_texturePath = TexturePath; }

		virtual void ConvertAssetToMyFormat(std::wstring FileName, bool isSkinnedData = false, bool NeedVertexInfo = false) override;
		virtual void ConvertAnimationAssetToMyFormat(std::wstring FileName) override;

	public:
		void Release();

		//-------Device------//
		ComPtr<ID3D12Device> GetDevice();

		//------Render-----//
		void BeginRender();
		void EndRender();
		void GraphicsPresent();
		uint32 GetFrameIndex() const;

		//------CommandList------//
		void CreateCommandList();
		ComPtr<ID3D12GraphicsCommandList> GetCommandList();

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

		// Test : 2024.05.07; Type마다 CBPool을 만들어야하나?
		void CreateCBPool();

		// 2024.05.08; 256의 512, 1024 로 만들어서 필요한 용량에 따라 사용해도 괜찮을 듯?
		std::shared_ptr<Ideal::D3D12ConstantBufferPool> GetCBPool(uint32 SizePerCB);
		std::shared_ptr<Ideal::D3D12ConstantBufferPool> GetCBBonePool();

		//------Camera------//
		void CreateDefaultCamera(); // default camera
		Matrix GetView();
		Matrix GetProj();
		Matrix GetViewProj();

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
		ComPtr<ID3D12Fence> m_graphicsFence = nullptr;
		uint64 m_graphicsFenceValue = 0;
		HANDLE m_graphicsFenceEvent = NULL;

		// RenderScene
		std::shared_ptr<Ideal::IdealRenderScene> m_currentRenderScene;

	private:
		// D3D12 Data Manager
		std::shared_ptr<Ideal::D3D12DescriptorHeap> m_descriptorHeap = nullptr;

		// 2024.05.08; 256의 512, 1024 로 만들어서 필요한 용량에 따라 사용해도 괜찮을 듯?
		std::shared_ptr<Ideal::D3D12ConstantBufferPool> m_cb256Pool = nullptr;
		std::shared_ptr<Ideal::D3D12ConstantBufferPool> m_cb512Pool = nullptr;
		std::shared_ptr<Ideal::D3D12ConstantBufferPool> m_cb1024Pool = nullptr;

		// bone의 데이터가 크다. 일단은 따로 만들다가 나중에 SRV로 넘겨주든 해야겠다.
		std::shared_ptr<Ideal::D3D12ConstantBufferPool> m_cbBonePool = nullptr;

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

	private:
		// 2024.05.14 : MRT Test
		std::shared_ptr<Ideal::D3D12Texture> t1 = nullptr;
	};
}