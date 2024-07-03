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

	class D3D12RayTracingRenderer;
	class DeferredDeleteManager;
}

namespace Ideal
{
	namespace GlobalRootSignature
	{
		namespace Slot
		{
			enum Enum
			{
				UAV_Output = 0,
				SRV_AccelerationStructure,
				CBV_Global,
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
		static const uint32 MAX_RAY_RECURSION_DEPTH = G_MAX_RAY_RECURSION_DEPTH;
		wchar_t* c_raygenShaderName = L"MyRaygenShader";
		wchar_t* c_closestHitShaderName = L"MyClosestHitShader";
		wchar_t* c_missShaderName = L"MyMissShader";
		wchar_t* c_hitGroupName = L"MyHitGroup";
	public:
		RaytracingManager();
		~RaytracingManager();

	public:
		void Init(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12Shader> RaytracingShader, std::shared_ptr<Ideal::D3D12Shader> AnimationShader, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, uint32 Width, uint32 Height);
		void DispatchRays(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, uint32 CurrentFrameIndex, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, SceneConstantBuffer SceneCB);
		void Resize(ComPtr<ID3D12Device5> Device, uint32 Width, uint32 Height);

		//---UAV Render Target---//
		void CreateUAVRenderTarget(ComPtr<ID3D12Device5> Device, const uint32& Width, const uint32& Height);
		ComPtr<ID3D12Resource> GetRaytracingOutputResource();

		//---AS---//
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> GetBLASByName(const std::wstring& Name);
		std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> AddBLAS(std::shared_ptr<Ideal::D3D12RayTracingRenderer> Renderer, ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, std::shared_ptr<Ideal::IMeshObject> MeshObject, const wchar_t* Name, bool IsSkinnedData = false);
		uint32 AllocateInstanceIndexByBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, uint32 InstanceContributionToHitGroupIndex = UINT_MAX, Matrix transform = Matrix::Identity, BYTE InstanceMask = 1);

		void SetGeometryTransformByIndex(uint32 InstanceIndex, const Matrix& Transform);
		void FinalCreate2(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, bool ForceBuild = false);

		void UpdateAccelerationStructures(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager);
		//ComPtr<ID3D12Resource> GetTLASResource();

		//---Root Singnature---//
		void CreateRootSignature(ComPtr<ID3D12Device5> Device);

		//---Shader Table---//
		void CreateRaytracingPipelineStateObject(
			ComPtr<ID3D12Device5> Device,
			std::shared_ptr<Ideal::D3D12Shader> Shader
		);
		void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
		void BuildShaderTables(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager);

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
		std::shared_ptr<Ideal::D3D12DescriptorHeap> m_uavSingleDescriptorHeap = nullptr;

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
	};
}
