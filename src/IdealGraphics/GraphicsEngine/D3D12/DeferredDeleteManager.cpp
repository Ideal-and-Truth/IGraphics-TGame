#include "DeferredDeleteManager.h"
#include "GraphicsEngine/public/IMeshObject.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/Resource/IdealMaterial.h"

Ideal::DeferredDeleteManager::DeferredDeleteManager()
{

}

Ideal::DeferredDeleteManager::~DeferredDeleteManager()
{

}

void Ideal::DeferredDeleteManager::DeleteDeferredResources(uint32 CurrentContextIndex)
{

#ifndef BeforeRefactor
	m_frameCount++;
	// ver3
	// 그냥 배열을 만들어 삭제한다.
	DeleteD3D12Resource(CurrentContextIndex);
	DeleteMeshObject(CurrentContextIndex);
	DeleteBLAS(CurrentContextIndex);
	DeleteTLAS(CurrentContextIndex);
#endif

#ifdef BeforeRefactor
	// ver2
	DeleteD3D12Resource(CurrentContextIndex);
	DeleteTexture(CurrentContextIndex);
	DeleteMaterial(CurrentContextIndex);
	DeleteMeshObject(CurrentContextIndex);
	DeleteBLAS(CurrentContextIndex);
	DeleteTLAS(CurrentContextIndex);
	m_currentContextIndex = CurrentContextIndex;
#endif

	// ver1
	//uint32 DeleteContextIndex = (m_currentContextIndex + MAX_PENDING_FRAMES - 1) % MAX_PENDING_FRAMES;
	//
	//DeleteD3D12Resource(DeleteContextIndex);
	//DeleteMeshObject(DeleteContextIndex);
	//DeleteBLAS(DeleteContextIndex);
	//DeleteTLAS(DeleteContextIndex);
	//
	//m_currentContextIndex = CurrentContextIndex;
}
void Ideal::DeferredDeleteManager::AddD3D12ResourceToDelete(ComPtr<ID3D12Resource> D3D12Resource)
{
#ifdef BeforeRefactor
	m_resourcesToDelete[m_currentContextIndex].push_back(D3D12Resource);
#else
	DeferredDeleteResource <ComPtr<ID3D12Resource>> Resource;
	Resource.Resource = D3D12Resource;
	Resource.ContextIndex = m_frameCount + G_SWAP_CHAIN_NUM;
	m_resourcesToDelete2.push(Resource);
#endif
}

void Ideal::DeferredDeleteManager::DeleteD3D12Resource(uint32 DeleteContextIndex)
{
#ifdef BeforeRefactor
	if (m_resourcesToDelete[DeleteContextIndex].size() > 0)
	{
		// 사실 ComPtr이라 Reset 안걸어줘도 됨
		for (auto& Resource : m_resourcesToDelete[DeleteContextIndex])
		{
			Resource.Reset();
		}
		m_resourcesToDelete[DeleteContextIndex].clear();
	}
#else
	while (!m_resourcesToDelete2.empty())
	{
		auto& Resource = m_resourcesToDelete2.front();
		if (Resource.ContextIndex == m_frameCount)
		{
			Resource.Resource.Reset();
			m_resourcesToDelete2.pop();
		}
		else
		{
			break;
		}
	}
#endif
}

void Ideal::DeferredDeleteManager::AddMeshObjectToDeferredDelete(std::shared_ptr<Ideal::IMeshObject>MeshObject)
{
#ifdef BeforeRefactor
	m_meshObjectsToDelete[m_currentContextIndex].push_back(MeshObject);
#else
	DeferredDeleteResource<std::shared_ptr<IMeshObject>> Resource;
	Resource.Resource = MeshObject;
	Resource.ContextIndex = m_frameCount + G_SWAP_CHAIN_NUM;
	m_meshObjectsToDelete2.push(Resource);
#endif
}

void Ideal::DeferredDeleteManager::DeleteMeshObject(uint32 DeleteContextIndex)
{
#ifdef BeforeRefactor
	if (m_meshObjectsToDelete[DeleteContextIndex].size() > 0)
	{
		for (auto& Resource : m_meshObjectsToDelete[DeleteContextIndex])
		{
			Resource.reset();
		}
		m_meshObjectsToDelete[DeleteContextIndex].clear();
	}
#else
	while (!m_meshObjectsToDelete2.empty())
	{
		auto& Resource = m_meshObjectsToDelete2.front();
		if (Resource.ContextIndex == m_frameCount)
		{
			Resource.Resource.reset();
			m_meshObjectsToDelete2.pop();
		}
		else
		{
			break;
		}
	}
#endif
}

void Ideal::DeferredDeleteManager::AddBLASToDeferredDelete(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS)
{
#ifdef BeforeRefactor
	m_blasToDelete[m_currentContextIndex].push_back(BLAS);
#else
	DeferredDeleteResource<std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure>> Resource;
	Resource.Resource = BLAS;
	Resource.ContextIndex = m_frameCount + G_SWAP_CHAIN_NUM;
	m_blasToDelete2.push(Resource);
#endif
}

void Ideal::DeferredDeleteManager::DeleteBLAS(uint32 DeleteContextIndex)
{
#ifdef BeforeRefactor
	if (m_blasToDelete[DeleteContextIndex].size() > 0)
	{
		for (auto& Resource : m_blasToDelete[DeleteContextIndex])
		{
			Resource->FreeMyHandle();
			Resource.reset();
		}
		m_blasToDelete[DeleteContextIndex].clear();
	}
#else
	while (!m_blasToDelete2.empty())
	{
		auto& Resource = m_blasToDelete2.front();
		if (Resource.ContextIndex == m_frameCount)
		{
			Resource.Resource->FreeMyHandle();
			Resource.Resource.reset();
			m_blasToDelete2.pop();
		}
		else
		{
			break;
		}
	}
#endif
}

void Ideal::DeferredDeleteManager::AddTLASToDeferredDelete(std::shared_ptr<Ideal::DXRTopLevelAccelerationStructure> TLAS)
{
#ifdef BeforeRefactor
	m_tlasToDelete[m_currentContextIndex].push_back(TLAS);
#else
	DeferredDeleteResource<std::shared_ptr<Ideal::DXRTopLevelAccelerationStructure>> Resource;
	Resource.Resource = TLAS;
	Resource.ContextIndex = m_frameCount + G_SWAP_CHAIN_NUM;
	m_tlasToDelete2.push(Resource);
#endif
}

void Ideal::DeferredDeleteManager::DeleteTLAS(uint32 DeleteContextIndex)
{
#ifdef BeforeRefactor
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
#else
	while (!m_tlasToDelete2.empty())
	{
		auto& Resource = m_tlasToDelete2.front();
		if (Resource.ContextIndex == m_frameCount)
		{
			Resource.Resource.reset();
			m_tlasToDelete2.pop();
		}
		else
		{
			break;
		}
	}
#endif
}

void Ideal::DeferredDeleteManager::AddTextureToDeferredDelete(std::shared_ptr<Ideal::D3D12Texture> Texture)
{
	m_textureToDelete[m_currentContextIndex].push_back(Texture);
}

void Ideal::DeferredDeleteManager::DeleteTexture(uint32 DeleteContextIndex)
{
	if (m_textureToDelete[DeleteContextIndex].size() > 0)
	{
		for (auto& Resource : m_textureToDelete[DeleteContextIndex])
		{
			Resource.reset();
		}
		m_textureToDelete[DeleteContextIndex].clear();
	}
}

void Ideal::DeferredDeleteManager::AddMaterialToDefferedDelete(std::shared_ptr<Ideal::IdealMaterial> Material)
{
	m_materialToDelete[m_currentContextIndex].push_back(Material);
}

void Ideal::DeferredDeleteManager::DeleteMaterial(uint32 DeleteContextIndex)
{
	if (m_materialToDelete[DeleteContextIndex].size() > 0)
	{
		for (auto& Resource : m_materialToDelete[DeleteContextIndex])
		{
			// 현재 Free는 RayTracing의 핸들 값을 해제함.
			Resource->FreeInRayTracing();
			Resource.reset();
		}
		m_materialToDelete[DeleteContextIndex].clear();
	}
}
