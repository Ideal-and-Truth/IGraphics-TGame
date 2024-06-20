#include "IdealRaytracingRenderScene.h"

#include "GraphicsEngine/D3D12/Raytracing/RaytracingManager.h"
#include "GraphicsEngine/D3D12/D3D12UploadBufferPool.h"

#include "GraphicsEngine/Resource/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/IdealScreenQuad.h"
#include "GraphicsEngine/Resource/Light/IdealDirectionalLight.h"
#include "GraphicsEngine/Resource/Light/IdealSpotLight.h"
#include "GraphicsEngine/Resource/Light/IdealPointLight.h"
#include <d3d12.h>

Ideal::IdealRaytracingRenderScene::IdealRaytracingRenderScene()
{

}

Ideal::IdealRaytracingRenderScene::~IdealRaytracingRenderScene()
{

}

void Ideal::IdealRaytracingRenderScene::Init(ComPtr<ID3D12Device5> InDevice, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool)
{
	m_device = InDevice;
	m_raytracingManager = std::make_shared<Ideal::RaytracingManager>();
	m_raytracingManager->Init();
	m_raytracingManager->FinalCreate(m_device, CommandList, UploadBufferPool);
}

void Ideal::IdealRaytracingRenderScene::Draw(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool)
{
	UpdateAccelerationStructures(CommandList, UploadBufferPool);

	for (auto& staticMeshObject : m_staticMeshObjects)
	{
		//staticMeshObject.lock()->Draw()
	}
}

void Ideal::IdealRaytracingRenderScene::UpdateAccelerationStructures(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool)
{
	m_raytracingManager->UpdateAccelerationStructures(CommandList, UploadBufferPool);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Ideal::IdealRaytracingRenderScene::GetTLASResource()
{
	return m_raytracingManager->GetTLASResource();
}

void Ideal::IdealRaytracingRenderScene::AddObject(std::shared_ptr<Ideal::IMeshObject> MeshObject)
{
	if (std::dynamic_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject) != nullptr)
	{
		std::shared_ptr<Ideal::IdealStaticMeshObject> staticMeshObject = std::static_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject);
		m_staticMeshObjects.push_back(staticMeshObject);

		// add blas
		staticMeshObject->AllocateBLASInstanceID(m_device, m_raytracingManager);
	}
	else if (std::dynamic_pointer_cast<Ideal::IdealSkinnedMeshObject>(MeshObject) != nullptr)
	{
		m_skinnedMeshObjects.push_back(std::static_pointer_cast<Ideal::IdealSkinnedMeshObject>(MeshObject));
	}
}

void Ideal::IdealRaytracingRenderScene::AddDebugObject(std::shared_ptr<Ideal::IMeshObject> MeshObject)
{
	if (std::dynamic_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject) != nullptr)
	{
		m_debugMeshObjects.push_back(std::static_pointer_cast<Ideal::IdealStaticMeshObject>(MeshObject));
	}
}

void Ideal::IdealRaytracingRenderScene::AddLight(std::shared_ptr<Ideal::ILight> Light)
{
	switch (Light->GetLightType())
	{
		case ELightType::None:
		{
			__debugbreak();
		}
		break;
		case ELightType::Directional:
		{
			// Directional Light는 그냥 바꿔준다.
			m_directionalLight = std::static_pointer_cast<IdealDirectionalLight>(Light);
		}
		break;
		case ELightType::Spot:
		{
			m_spotLights.push_back(std::static_pointer_cast<IdealSpotLight>(Light));
		}
		break;
		case ELightType::Point:
		{
			m_pointLights.push_back(std::static_pointer_cast<IdealPointLight>(Light));
		}
		break;
		default:
		{

		}
		break;

	}
}
