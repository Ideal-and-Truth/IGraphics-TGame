#pragma once
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include <winnt.h>

#include "GraphicsEngine/VertexInfo.h"

// 따로 GPU에 메모리를 업로드 하는 command list를 파서 여기서 사용한다.

namespace Ideal
{
	class D3D12Resource;
	class D3D12VertexBuffer;
	class D3D12IndexBuffer;
	class D3D12ShaderResourceView;
	class D3D12UnorderedAccessView;
	class D3D12Texture;
	class D3D12DescriptorHeap;
	class IdealStaticMeshObject;
	class IdealSkinnedMeshObject;
	template <typename> class IdealMesh;
	class IdealMaterial;
	class IdealStaticMesh;
	class IdealSkinnedMesh;
	class IdealAnimation;
	class RaytracingManager;
	class DeferredDeleteManager;
}

namespace Ideal
{
	typedef
		enum IdealTextureTypeFlag
	{
		IDEAL_TEXTURE_NONE = 0,
		IDEAL_TEXTURE_SRV = 0x1,
		IDEAL_TEXTURE_RTV = 0x2,
		IDEAL_TEXTURE_DSV = 0x4,
		IDEAL_TEXTURE_UAV = 0x8,
		// 0x1,0x2,0x4,0x8
		// 0x10,0x20,0x40,0x80
	} IdealTextureTypeFlag;
	DEFINE_ENUM_FLAG_OPERATORS(IdealTextureTypeFlag);

	typedef
		enum IdealAllocateTypeFlag
	{
		IDEAL_BUFFER_ALLOCATE_DEFAULT = 0,
		IDEAL_BUFFER_ALLOCATE_SRV = 0x1,
	} IdealAllocateTypeFlag;
	DEFINE_ENUM_FLAG_OPERATORS(IdealAllocateTypeFlag);

	class ResourceManager : public std::enable_shared_from_this<ResourceManager>
	{
	private:
		//--resource id--//
		uint64 AllocateMaterialID();
		uint64 m_materialID = 0;

	public:
		ResourceManager();
		virtual ~ResourceManager();

	public:
		void SetAssetPath(const std::wstring& AssetPath) { m_assetPath = AssetPath; }
		void SetModelPath(const std::wstring& ModelPath) { m_modelPath = ModelPath; }
		void SetTexturePath(const std::wstring& TexturePath) { m_texturePath = TexturePath; }

	public:
		void Init(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager);
		void Fence();
		void WaitForFenceValue();

		ComPtr<ID3D12DescriptorHeap> GetSRVHeap() { return m_cbv_srv_uavHeap->GetDescriptorHeap(); }
		std::shared_ptr<Ideal::D3D12DynamicDescriptorHeap> GetSRVPool() { return m_cbv_srv_uavHeap; }

		void CreateVertexBufferBox(std::shared_ptr<Ideal::D3D12VertexBuffer>& VertexBuffer);
		void CreateIndexBufferBox(std::shared_ptr<Ideal::D3D12IndexBuffer>& IndexBuffer);

		template <typename TVertexType>
		void CreateVertexBuffer(std::shared_ptr<Ideal::D3D12VertexBuffer> OutVertexBuffer,
			std::vector<TVertexType>& Vertices
		)
		{
			m_commandAllocator->Reset();
			m_commandList->Reset(m_commandAllocator.Get(), nullptr);

			const uint32 elementSize = sizeof(TVertexType);
			const uint32 elementCount = (uint32)Vertices.size();
			const uint32 bufferSize = elementSize * elementCount;

			Ideal::D3D12UploadBuffer uploadBuffer;
			uploadBuffer.Create(m_device.Get(), bufferSize);
			{
				void* mappedData = uploadBuffer.Map();
				memcpy(mappedData, Vertices.data(), bufferSize);
				uploadBuffer.UnMap();
			}
			OutVertexBuffer->Create(m_device.Get(),
				m_commandList.Get(),
				elementSize,
				elementCount,
				uploadBuffer
			);

			//---------Execute---------//
			m_commandList->Close();
			ID3D12CommandList* commandLists[] = { m_commandList.Get() };
			m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

			Fence();
			WaitForFenceValue();
		}

		void CreateIndexBuffer(std::shared_ptr<Ideal::D3D12IndexBuffer> OutIndexBuffer,
			std::vector<uint32>& Indices
		);
		void CreateIndexBufferUint16(std::shared_ptr<Ideal::D3D12IndexBuffer> OutIndexBuffer,
			std::vector<uint16>& Indices);

		// 파일 로드하여 srv로 만든다.
		void CreateTexture(std::shared_ptr<Ideal::D3D12Texture>& OutTexture, const std::wstring& Path, bool IgnoreSRGB= false);

		void CreateTextureDDS(std::shared_ptr<Ideal::D3D12Texture>& OutTexture, const std::wstring& Path);

		void CreateDynamicTexture(std::shared_ptr<Ideal::D3D12Texture>& OutTexture, uint32 Width, uint32 Height);

		// 2024.06.03 : Refactor : Create Texture with flag
		void CreateEmptyTexture2D(std::shared_ptr<Ideal::D3D12Texture>& OutTexture, const uint32& Width, const uint32 Height, DXGI_FORMAT Format, const Ideal::IdealTextureTypeFlag& TextureFlags, const std::wstring& Name);

		// 2024.06.10 : Create Texture2D by already Committed Resource
		//void CreateSRV(std::shared_ptr<Ideal::D3D12ShaderResourceView> OutSRV, uint32);
		std::shared_ptr<Ideal::D3D12ShaderResourceView> CreateSRV(std::shared_ptr<Ideal::D3D12Resource> Resource, uint32 NumElements, uint32 ElementSize);
		std::shared_ptr<Ideal::D3D12ShaderResourceView> CreateSRV(ComPtr<ID3D12Resource> Resource, uint32 NumElements, uint32 ElementSize);
		std::shared_ptr<Ideal::D3D12UnorderedAccessView> CreateUAV(std::shared_ptr<Ideal::D3D12Resource> Resource, uint32 NumElements, uint32 ElementSize);

		void CreateStaticMeshObject(std::shared_ptr<Ideal::IdealStaticMeshObject> OutMesh, const std::wstring& filename);
		void CreateSkinnedMeshObject(std::shared_ptr<Ideal::IdealSkinnedMeshObject> OutMesh, const std::wstring& filename);
		void CreateAnimation(std::shared_ptr<Ideal::IdealAnimation>& OutAnimation, const std::wstring& filename, const Matrix& offset = Matrix::Identity);

		void DeleteStaticMeshObject(std::shared_ptr<Ideal::IdealStaticMeshObject> Mesh);
		void DeleteSkinnedMeshObject(std::shared_ptr<Ideal::IdealSkinnedMeshObject> Mesh);

		D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHeap() { return m_rtvHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(); };

		std::shared_ptr<Ideal::IdealMaterial> CreateMaterial();

		void DeleteTexture(std::shared_ptr<Ideal::D3D12Texture> Texture);

	private:
		ComPtr<ID3D12Device5> m_device = nullptr;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator = nullptr;
		ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
		ComPtr<ID3D12GraphicsCommandList4> m_commandList = nullptr;

		ComPtr<ID3D12Fence> m_fence = nullptr;
		uint64 m_fenceValue = 0;
		HANDLE m_fenceEvent = NULL;

		std::shared_ptr<Ideal::DeferredDeleteManager> m_deferredDeleteManager;

	private:
		// Descriptor heaps
		std::shared_ptr<Ideal::D3D12DynamicDescriptorHeap> m_cbv_srv_uavHeap;
		const uint32 m_srvHeapCount = 4096;

		// 2024.05.14 Multi Render Target
		std::shared_ptr<Ideal::D3D12DynamicDescriptorHeap> m_rtvHeap;
		std::shared_ptr<Ideal::D3D12DynamicDescriptorHeap> m_dsvHeap;

	private:
		std::wstring m_assetPath;
		std::wstring m_modelPath;
		std::wstring m_texturePath;

		std::map<std::string, std::shared_ptr<Ideal::IdealStaticMesh>> m_staticMeshes;
		std::map<std::string, std::shared_ptr<Ideal::IdealSkinnedMesh>> m_skinnedMeshes;
		std::map<std::string, std::shared_ptr<Ideal::IdealAnimation>> m_animations;
		std::map<std::string, std::shared_ptr<Ideal::D3D12Texture>> m_textures;

	public:
		void CreateDefaultTextures();
		void CreateDefaultMaterial();
		std::shared_ptr<Ideal::IdealMaterial> GetDefaultMaterial();
		std::shared_ptr<Ideal::D3D12Texture> GetDefaultAlbedoTexture();
		std::shared_ptr<Ideal::D3D12Texture> GetDefaultNormalTexture();
		std::shared_ptr<Ideal::D3D12Texture> GetDefaultMaskTexture();

	private:
		std::shared_ptr<Ideal::D3D12Texture> m_defaultAlbedo;
		std::shared_ptr<Ideal::D3D12Texture> m_defaultNormal;
		std::shared_ptr<Ideal::D3D12Texture> m_defaultMask;

		std::shared_ptr<Ideal::IdealMaterial> m_defaultMaterial;

	public:
		void CreateDefaultQuadMesh();
		std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> GetDefaultQuadMesh();
		void CreateDefaultQuadMesh2(); // 시작 위치가 다르다
		std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> GetDefaultQuadMesh2();

	private:
		std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> m_defaultQuadMesh;
		std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> m_defaultQuadMesh2;
	};
}