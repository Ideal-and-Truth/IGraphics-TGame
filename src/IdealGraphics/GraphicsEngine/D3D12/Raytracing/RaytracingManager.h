#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

// test
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructureManager.h"

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
	class D3D12DynamicConstantBufferAllocator;
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

	/// <summary>
	/// �ؾ� �� ��� ����
	/// Mesh Object�� ���鶧���� BLAS�� �����ؾ� �Ѵ�.
	///		-> BLAS ���� �� TLAS ������
	/// Shader Table ������ �ؾ��� �� ����.
	/// BLAS TLAS�� ����� �Ǵ� ������Ʈ�� �ؾ��Ѵ�.
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
		void Init(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager, std::shared_ptr<Ideal::D3D12Shader> Shader);
		void DispatchRays(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, Ideal::D3D12DescriptorHandle OutputUAVHandle, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, const uint32& Width, const uint32& Height, SceneConstantBuffer SceneCB);

		//---AS---//
		uint32 AddBLASAndGetInstanceIndex(ComPtr<ID3D12Device5> Device, std::vector<BLASGeometry>& Geometries, const wchar_t* Name);
		void SetGeometryTransformByIndex(uint32 InstanceIndex, const Matrix& Transform);
		void FinalCreate(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool);
		void UpdateAccelerationStructures(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool);
		ComPtr<ID3D12Resource> GetTLASResource();

		//---Root Singnature---//
		void CreateRootSignature(ComPtr<ID3D12Device5> Device);

		//---Shader Table---//
		void CreateRaytracingPipelineStateObject(
			ComPtr<ID3D12Device5> Device,
			std::shared_ptr<Ideal::D3D12Shader> Shader
		);
		void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
		void BuildShaderTables(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager);

		//TEMP
		ComPtr<ID3D12Resource> GetRayGenShaderTable() { return m_rayGenShaderTable; }
		const std::map<std::wstring, std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure>> GetBLASes() { return m_ASManager->GetBLASes(); }

	private:
		std::shared_ptr<Ideal::D3D12DynamicDescriptorHeap> m_shaderTableHeap;

		std::unique_ptr<DXRAccelerationStructureManager> m_ASManager;

		ComPtr<ID3D12RootSignature> m_raytracingGlobalRootSignature;
		ComPtr<ID3D12RootSignature> m_raytracingLocalRootSignature;
		ComPtr<ID3D12StateObject> m_dxrStateObject;

		// Shader Table
		ComPtr<ID3D12Resource> m_missShaderTable;
		ComPtr<ID3D12Resource> m_rayGenShaderTable;
		ComPtr<ID3D12Resource> m_hitGroupShaderTable;
		uint64 m_hitGroupShaderTableStrideInBytes;
		uint64 m_missShaderTableStrideInBytes;
		// temp : �ӽ÷� �ڵ��� �ٹ޾Ƶΰ���
		std::vector<Ideal::D3D12DescriptorHandle> handles;
		std::vector<std::shared_ptr<Ideal::D3D12ShaderResourceView>> srvs;

		// Shader Table Record�� �ε���
		// �� BLAS���� Geometry�� ������ �ٸ��״� Add Instance �� ������ 
		// ���� HitGroupIndex�� instance�� �־��ְ�
		// �ش� BLAS�� ����ִ� Geometry�� ������ŭ
		// �÷��ְ� ������.
		// �̷����� ���� Instance�� �ٽ� �ڱ⸸�� instance�� ���� �� �ְ� �ȴ�.
		uint64 m_contributionToHitGroupIndexCount = 0;
	};
}
