#include "DeferredDeleteManager.h"
#include "GraphicsEngine/public/IMeshObject.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"

Ideal::DeferredDeleteManager::DeferredDeleteManager()
{

}

Ideal::DeferredDeleteManager::~DeferredDeleteManager()
{

}

void Ideal::DeferredDeleteManager::DeleteDeferredResources(uint32 CurrentContextIndex)
{
	
	uint32 DeleteContextIndex = (m_currentContextIndex + 2) % MAX_PENDING_FRAMES;
	//uint32 DeleteContextIndex = m_currentContextIndex;

	DeleteD3D12Resource(DeleteContextIndex);
	DeleteMeshObject(DeleteContextIndex);
	DeleteBLAS(DeleteContextIndex);
	DeleteTLAS(DeleteContextIndex);

	m_currentContextIndex = CurrentContextIndex;
}

void Ideal::DeferredDeleteManager::AddD3D12ResourceToDelete(ComPtr<ID3D12Resource> Resource)
{
	m_resourcesToDelete[m_currentContextIndex].push_back(Resource);
}

void Ideal::DeferredDeleteManager::DeleteD3D12Resource(uint32 DeleteContextIndex)
{
	if (m_resourcesToDelete[DeleteContextIndex].size() > 0)
	{
		// ªÁΩ« ComPtr¿Ã∂Û Reset æ»∞…æÓ¡‡µµ µ 
		for (auto& Resource : m_resourcesToDelete[DeleteContextIndex])
		{
			Resource.Reset();
		}
		m_resourcesToDelete[DeleteContextIndex].clear();
	}
}

void Ideal::DeferredDeleteManager::AddMeshObjectToDeferredDelete(std::shared_ptr<Ideal::IMeshObject>MeshObject)
{
	m_meshObjectsToDelete[m_currentContextIndex].push_back(MeshObject);
}

void Ideal::DeferredDeleteManager::DeleteMeshObject(uint32 DeleteContextIndex)
{
	if (m_meshObjectsToDelete[DeleteContextIndex].size() > 0)
	{
		for (auto& Resource : m_meshObjectsToDelete[DeleteContextIndex])
		{
			Resource.reset();
		}
		m_meshObjectsToDelete[DeleteContextIndex].clear();
	}
}

void Ideal::DeferredDeleteManager::AddBLASToDeferredDelete(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS)
{
	m_blasToDelete[m_currentContextIndex].push_back(BLAS);
}

void Ideal::DeferredDeleteManager::DeleteBLAS(uint32 DeleteContextIndex)
{
	if (m_blasToDelete[DeleteContextIndex].size() > 0)
	{
		for (auto& Resource : m_blasToDelete[DeleteContextIndex])
		{
			Resource->FreeMyHandle();
			Resource.reset();
		}
		m_blasToDelete[DeleteContextIndex].clear();
	}
}

void Ideal::DeferredDeleteManager::AddTLASToDeferredDelete(std::shared_ptr<Ideal::DXRTopLevelAccelerationStructure> TLAS)
{
	m_tlasToDelete[m_currentContextIndex].push_back(TLAS);
}

void Ideal::DeferredDeleteManager::DeleteTLAS(uint32 DeleteContextIndex)
{
	// TODO Delete
	if (m_tlasToDelete[DeleteContextIndex].size() > 0)
	{
		for (auto& Resource : m_tlasToDelete[DeleteContextIndex])
		{
			//Resource->FreeMyHandle();
			Resource.reset();
		}
		m_tlasToDelete[DeleteContextIndex].clear();
	}
}
