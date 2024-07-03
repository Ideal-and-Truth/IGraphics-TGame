#include "DeferredDeleteManager.h"


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

	if (m_resourcesToDelete[DeleteContextIndex].size() == 0)
		return;

	// ªÁΩ« ComPtr¿Ã∂Û Reset æ»∞…æÓ¡‡µµ µ 
	for (auto& Resource : m_resourcesToDelete[DeleteContextIndex])
	{
		Resource.Reset();
		//m_resourcesToDelete[DeleteContextIndex].clear();
	}
	m_resourcesToDelete[DeleteContextIndex].clear();
}

void Ideal::DeferredDeleteManager::AddResourceToDelete(ComPtr<ID3D12Resource> Resource)
{
	m_resourcesToDelete[m_currentContextIndex].push_back(Resource);
}
