#include "DeferredDeleteManager.h"
#include "GraphicsEngine/public/IMeshObject.h"

Ideal::DeferredDeleteManager::DeferredDeleteManager()
{

}

Ideal::DeferredDeleteManager::~DeferredDeleteManager()
{

}

void Ideal::DeferredDeleteManager::DeleteDeferredResources(uint32 CurrentContextIndex)
{
	m_currentContextIndex = CurrentContextIndex;
	
	uint32 DeleteContextIndex = (m_currentContextIndex + 1) % MAX_PENDING_FRAMES;

	if (m_resourcesToDelete[DeleteContextIndex].size() > 0)
	{
		// »ç½Ç ComPtrÀÌ¶ó Reset ¾È°É¾îÁàµµ µÊ
		for (auto& Resource : m_resourcesToDelete[DeleteContextIndex])
		{
			Resource.Reset();
		}
		m_resourcesToDelete[DeleteContextIndex].clear();
	}

	if (m_meshObjectsToDelete[DeleteContextIndex].size() > 0)
	{
		for (auto& Resource : m_meshObjectsToDelete[DeleteContextIndex])
		{
			Resource.reset();
		}
		m_meshObjectsToDelete[DeleteContextIndex].clear();
	}
}

void Ideal::DeferredDeleteManager::AddResourceToDelete(ComPtr<ID3D12Resource> Resource)
{
	m_resourcesToDelete[m_currentContextIndex].push_back(Resource);
}

void Ideal::DeferredDeleteManager::DeleteDeferredMeshObject(std::shared_ptr<Ideal::IMeshObject>MeshObject)
{
	m_meshObjectsToDelete[m_currentContextIndex].push_back(MeshObject);
}

void Ideal::DeferredDeleteManager::AddMeshObjectToDelete(std::shared_ptr<Ideal::IMeshObject>MeshObject)
{
	m_meshObjectsToDelete[m_currentContextIndex].push_back(MeshObject);
}
