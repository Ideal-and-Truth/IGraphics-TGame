#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include <d3d12.h>
#include <d3dx12.h>

namespace Ideal
{
	class IMeshObject;
	class DXRBottomLevelAccelerationStructure;
	class DXRTopLevelAccelerationStructure;
}
namespace Ideal
{
	template<typename T>
	struct DeferredDeleteResource
	{
		T Resource;
		uint32 ContextIndex;
	};
	
	/*struct DeferredDeleteResource
	{
		ComPtr<ID3D12Resource> Resource;
		uint32 ContextIndex;
	};*/

	class DeferredDeleteManager
	{
		static const uint32 MAX_PENDING_FRAMES = G_SWAP_CHAIN_NUM - 1;

	public:
		DeferredDeleteManager();
		~DeferredDeleteManager();

	public:
		void DeleteDeferredResources(uint32 CurrentContextIndex);

		void AddD3D12ResourceToDelete(ComPtr<ID3D12Resource> D3D12Resource);
		void DeleteD3D12Resource(uint32 DeleteContextIndex);

		void AddMeshObjectToDeferredDelete(std::shared_ptr<Ideal::IMeshObject>MeshObject);
		void DeleteMeshObject(uint32 DeleteContextIndex);

		void AddBLASToDeferredDelete(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS);
		void DeleteBLAS(uint32 DeleteContextIndex);

		void AddTLASToDeferredDelete(std::shared_ptr<Ideal::DXRTopLevelAccelerationStructure> TLAS);
		void DeleteTLAS(uint32 DeleteContextIndex);

	private:
		uint32 m_currentContextIndex = 0;

		std::vector<ComPtr<ID3D12Resource>> m_resourcesToDelete[MAX_PENDING_FRAMES];
		std::vector<std::shared_ptr<IMeshObject>> m_meshObjectsToDelete[MAX_PENDING_FRAMES];
		std::vector<std::shared_ptr<DXRBottomLevelAccelerationStructure>> m_blasToDelete[MAX_PENDING_FRAMES];
		std::vector<std::shared_ptr<Ideal::DXRTopLevelAccelerationStructure>> m_tlasToDelete[MAX_PENDING_FRAMES];

		// ver3
		uint64 m_frameCount = 0;
		std::queue<DeferredDeleteResource<ComPtr<ID3D12Resource>>> m_resourcesToDelete2;
		std::queue<DeferredDeleteResource<std::shared_ptr<IMeshObject>>> m_meshObjectsToDelete2;
		std::queue<DeferredDeleteResource<std::shared_ptr<DXRBottomLevelAccelerationStructure>>> m_blasToDelete2;
		std::queue<DeferredDeleteResource<std::shared_ptr<Ideal::DXRTopLevelAccelerationStructure>>> m_tlasToDelete2;
	};
}