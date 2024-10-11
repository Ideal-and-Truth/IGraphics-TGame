#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

// test
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructureManager.h"
#include "IMeshObject.h"

struct ID3D12Device5;
struct ID3D12GraphicsCommandList4;

namespace Ideal
{
	class DXRAccelerationStructureManager;
	class D3D12UploadBufferPool;
	class D3D12Shader;
	class ResourceManager;
	class D3D12ShaderResourceView;
	class D3D12DynamicDescriptorHeap;
	class D3D12DescriptorHeap;
	class D3D12DynamicConstantBufferAllocator;
	class D3D12DescriptorManager;
	class D3D12UnorderedAccessView;
	class D3D12Texture;

	class D3D12RayTracingRenderer;
	class DeferredDeleteManager;
}

namespace Ideal
{
	struct GBuffer
	{
		float tHit;
		Vector3 hitPosition;
	};

	struct RayPayload
	{
		uint32 rayRecursionDepth;
		Vector3 radiance;
		GBuffer gBuffer;
	};

	struct ShadowRayPayload
	{
		float tHit;
	};

	namespace PathtracerRayType
	{
		enum Enum
		{
			Radiance = 0,
			Shadow,
			Count
		};
	}

	namespace GlobalRootSignature
	{
		namespace Slot
		{
			enum Enum
			{
				UAV_Output = 0,
				SRV_AccelerationStructure,
				CBV_Global,
				SRV_SkyBox,
				CBV_LightList,

				UAV_GBufferPosition,
				UAV_GBufferDepth,

				SRV_PrevFrameBottomLevelASIstanceTransforms,
				Count
			};
		}
	}

	namespace LocalRootSignature
	{
		namespace Slot
		{
			enum Enum
			{
				SRV_IndexBuffer,
				SRV_VertexBuffer,
				SRV_Diffuse,
				SRV_Normal,
				//SRV_Metalic,
				//SRV_Roughness,
				SRV_Mask,
				CBV_MaterialInfo,
				SRV_PrevFrameVertexBuffer,
				Count
			};
		}
		struct RootArgument
		{
			// Index
			D3D12_GPU_DESCRIPTOR_HANDLE SRV_Indices;
			// Vertex
			D3D12_GPU_DESCRIPTOR_HANDLE SRV_Vertices;
			// Diffuse Texture
			D3D12_GPU_DESCRIPTOR_HANDLE SRV_DiffuseTexture;
			// Normal Textures
			D3D12_GPU_DESCRIPTOR_HANDLE SRV_NormalTexture;
			// Mask Texture
			D3D12_GPU_DESCRIPTOR_HANDLE SRV_MaskTexture;

			//D3D12_GPU_DESCRIPTOR_HANDLE CBV_MaterialInfo;
			CB_MaterialInfo CBV_MaterialInfo;

			// Prev Frame Vertex Buffer
			D3D12_GPU_DESCRIPTOR_HANDLE SRV_PrevFrameVertices;
		};
	}

	namespace AnimationRootSignature
	{
		namespace Slot
		{
			enum Enum
			{
				SRV_Vertices = 0,
				CBV_BoneData,
				CBV_VertexCount,
				UAV_OutputVertices,
				Count
			};
		}

	}

	struct InstanceInfo
	{
		uint32 InstanceIndex;
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS;
	};

	/// <summary>
	/// 해야 할 기능 정리
	/// Mesh Object를 만들때마다 BLAS를 생성해야 한다.
	///		-> BLAS 생성 및 TLAS 리빌드
	/// Shader Table 관리도 해야할 것 같음.
	/// BLAS TLAS를 재빌드 또는 업데이트를 해야한다.
	/// 
	/// </summary>
	class RaytracingManager
	{
		const static uint32 MAX_PENDING_FRAME = G_SWAP_CHAIN_NUM - 1;
		static const uint32 MAX_RAY_RECURSION_DEPTH = G_MAX_RAY_RECURSION_DEPTH;
		const wchar_t* c_raygenShaderName = L"MyRaygenShader";
		const wchar_t* c_closestHitShaderName[2] = { L"MyClosestHitShader", L"MyClosestHitShader_ShadowRay" };
		const wchar_t* c_missShaderName[2] = { L"MyMissShader", L"MyMissShader_ShadowRay" };
		const wchar_t* c_hitGroupName[2] = { L"MyHitGroup", L"MyHitGroup_ShadowRay" };

	public:
		RaytracingManager();
		~RaytracingManager();

	public:
		void Init(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12Shader> RaytracingShader, std::shared_ptr<Ideal::D3D12Shader> AnimationShader, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, uint32 Width, uint32 Height, uint32 MaxDrawPerCount);
		void DispatchRays(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, uint32 CurrentFrameIndex, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, SceneConstantBuffer SceneCB, CB_LightList* LightCB, std::shared_ptr<Ideal::D3D12Texture> SkyBoxTexture);
		void Resize(std::shared_ptr<Ideal::ResourceManager> ResourceManager, ComPtr<ID3D12Device5> Device, uint32 Width, uint32 Height);

		//---UAV Render Target---//
		void CreateRenderTarget(ComPtr<ID3D12Device5> Device, const uint32& Width, const uint32& Height);
		ComPtr<ID3D12Resource> GetRaytracingOutputResource();

		Ideal::D3D12DescriptorHandle GetRaytracingOutputRTVHandle();
		Ideal::D3D12DescriptorHandle GetRaytracingOutputSRVHandle();

		//---AS---//
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> GetBLASByName(const std::wstring& Name);
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> AddBLAS(std::shared_ptr<Ideal::D3D12RayTracingRenderer> Renderer, ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, std::shared_ptr<Ideal::IMeshObject> MeshObject, const wchar_t* Name, bool IsSkinnedData = false);
		std::shared_ptr<Ideal::BLASInstanceDesc> AllocateInstanceByBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, uint32 InstanceContributionToHitGroupIndex = UINT_MAX, Matrix transform = Matrix::Identity, BYTE InstanceMask = 1);

		//void DeleteBLASByName(const std::wstring& Name);
		void DeleteBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, const std::wstring& Name, bool IsSkinnedData);
		void DeleteBLASInstance(std::shared_ptr<Ideal::BLASInstanceDesc> Instance);

		void SetGeometryTransformByIndex(uint32 InstanceIndex, const Matrix& Transform);
		void FinalCreate2(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, bool ForceBuild = false);

		void UpdateAccelerationStructures(std::shared_ptr<Ideal::D3D12RayTracingRenderer> Renderer, ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager);
		//ComPtr<ID3D12Resource> GetTLASResource();

		//---Root Singnature---//
		void CreateRootSignature(ComPtr<ID3D12Device5> Device);

		//---Shader Table---//
		void CreateRaytracingPipelineStateObject(
			ComPtr<ID3D12Device5> Device,
			std::shared_ptr<Ideal::D3D12Shader> Shader
		);
		void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
		void BuildShaderTables(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager);

		//TEMP
		ComPtr<ID3D12Resource> GetRayGenShaderTable() { return m_rayGenShaderTable; }
		//const std::map<std::wstring, std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure>> GetBLASes() { return m_ASManager->GetBLASes(); }

	private:
		std::unique_ptr<DXRAccelerationStructureManager> m_ASManager = nullptr;

		//---Device---//
		uint32 m_width = 0;
		uint32 m_height = 0;

		//---UAV Render Target---//
		ComPtr<ID3D12Resource> m_raytracingOutput = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE m_raytacingOutputResourceUAVCpuDescriptorHandle;
		std::shared_ptr<Ideal::D3D12DescriptorHeap> m_raytracingOutputDescriptorHeap = nullptr;
		std::shared_ptr<Ideal::D3D12DescriptorHeap> m_RTV_raytracingOutputDescriptorHeap = nullptr;
		Ideal::D3D12DescriptorHandle m_raytracingOutputSRV;
		Ideal::D3D12DescriptorHandle m_raytracingOutputRTV;

		// 2024.08.12 Render Target //
		std::shared_ptr<Ideal::D3D12Texture> m_raytracingOutput2 = nullptr;

		//---Root Signature---//
		ComPtr<ID3D12RootSignature> m_raytracingGlobalRootSignature = nullptr;
		ComPtr<ID3D12RootSignature> m_raytracingLocalRootSignature = nullptr;
		ComPtr<ID3D12StateObject> m_dxrStateObject = nullptr;

		//---Shader Table---//
		ComPtr<ID3D12Resource> m_missShaderTable = nullptr;
		ComPtr<ID3D12Resource> m_rayGenShaderTable = nullptr;
		ComPtr<ID3D12Resource> m_hitGroupShaderTable = nullptr;
		uint64 m_hitGroupShaderTableStrideInBytes = 0;
		uint64 m_missShaderTableStrideInBytes = 0;

		// Shader Table Record의 인덱스
		// 각 BLAS마다 Geometry의 개수가 다를테니 Add Instance 할 때마다 
		// 현재 HitGroupIndex를 instance에 넣어주고
		// 해당 BLAS의 들어있는 Geometry의 개수만큼
		// 늘려주고 끝낸다.
		// 이로인해 다음 Instance는 다시 자기만의 instance를 가질 수 있게 된다.
		uint64 m_contributionToHitGroupIndexCount = 0;

		//---Material Texture---//
	public:
		// Mateiral이 바뀌었는지 검사한다.
		void UpdateMaterial(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager);
		// Material의 텍스쳐가 바뀌었는지를 검사한다.
		void UpdateTexture(ComPtr<ID3D12Device5> Device);
		void CreateMaterialInRayTracing(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, std::weak_ptr<Ideal::IdealMaterial> NewMaterial);
		void DeleteMaterialInRayTracing(std::shared_ptr<Ideal::IMaterial> Material);
	private:
		// 중복되는 Material 관리..?
		std::unordered_map<uint64, std::weak_ptr<Ideal::IdealMaterial>> m_materialMapInFixedDescriptorTable;


		//---Animation Compute Shader---//
	public:
		void CreateAnimationRootSignature(ComPtr<ID3D12Device5> Device);
		void CreateAnimationCSPipelineState(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::D3D12Shader> AnimationShader);

		void DispatchAnimationComputeShader(
			ComPtr<ID3D12Device5> Device,
			ComPtr<ID3D12GraphicsCommandList4> CommandList,
			std::shared_ptr<Ideal::ResourceManager> ResourceManager,
			std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager,
			uint32 CurrentContextIndex,
			std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool,
			std::shared_ptr<Ideal::D3D12VertexBuffer> VertexBuffer,
			CB_Bone* BoneData,
			std::shared_ptr<Ideal::D3D12UnorderedAccessView> UAV_OutVertex
		);

	private:
		ComPtr<ID3D12RootSignature> m_animationRootSignature;
		ComPtr<ID3D12PipelineState> m_animationPipelineState;

		//---GBuffer Texture---//
	public:
		void CreateGBufferTexture(std::shared_ptr<Ideal::ResourceManager> ResourceManager, uint32 Width, uint32 Height);
		std::shared_ptr<Ideal::D3D12Texture> GetDepthBuffer();

		void CopyDepthBuffer( ComPtr<ID3D12GraphicsCommandList4> CommandList);

	private:
		std::shared_ptr<Ideal::D3D12Texture> m_gBufferPosition;
		std::shared_ptr<Ideal::D3D12Texture> m_gBufferDepth;

		std::shared_ptr<Ideal::D3D12Texture> m_CopyDepthBuffer;	// raytracing으로 뽑은 depth buffer를 옮길 dsv
																// 이유는 uav와 dsv 동시 허용하여 생성을 못한다.
	
		//---PrevFrameBottomLevelAccelerationStructureInstanceTransform---//
		// velocity를 구하기위한 여정..
	public:
		void InitPrevFrameBuffer(ComPtr<ID3D12Device5> Device, uint32 MaxDrawCountPerFrame);
		void UpdatePrevFrameBuffer(uint32 CurrentContextIndex);

	private:
		std::shared_ptr<Ideal::D3D12UploadBufferPool> m_prevFrameBottomLevelASInstanceTransforms[MAX_PENDING_FRAME];
		std::vector<Matrix> m_prevFrameInstanceTransforms;
	};
}
