#pragma once
#include "Core/Core.h"
#include "IRenderScene.h"

struct ID3D12Device5;
struct ID3D12GraphicsCommandList4;
struct ID3D12Resource;

namespace Ideal
{
	class IdealStaticMeshObject;
	class IdealSkinnedMeshObject;
	class IdealDirectionalLight;
	class IdealSpotLight;
	class IdealPointLight;
	class RaytracingManager;
	class D3D12UploadBufferPool;
}

namespace Ideal
{
	class IdealRaytracingRenderScene : public IRenderScene
	{
	public:
		IdealRaytracingRenderScene();
		virtual ~IdealRaytracingRenderScene();

	public:
		void Init(ComPtr<ID3D12Device5> InDevice, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool);
		void Draw(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool);
		void UpdateAccelerationStructures(ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool);

		// Temp
		ComPtr<ID3D12Resource> GetTLASResource();
	public:
		//----------IRenderScene Interface-----------//
		virtual void AddObject(std::shared_ptr<Ideal::IMeshObject> MeshObject) override;
		virtual void AddDebugObject(std::shared_ptr<Ideal::IMeshObject> MeshObject) override;
		virtual void AddLight(std::shared_ptr<Ideal::ILight> Light) override;

	private:
		std::vector<std::weak_ptr<Ideal::IdealStaticMeshObject>> m_staticMeshObjects;
		std::vector<std::weak_ptr<Ideal::IdealSkinnedMeshObject>> m_skinnedMeshObjects;
		std::vector<std::weak_ptr<Ideal::IdealStaticMeshObject>> m_debugMeshObjects;

		std::weak_ptr<Ideal::IdealDirectionalLight> m_directionalLight;
		std::vector<std::weak_ptr<Ideal::IdealSpotLight>> m_spotLights;
		std::vector<std::weak_ptr<Ideal::IdealPointLight>> m_pointLights;

	private:
		std::shared_ptr<Ideal::RaytracingManager> m_raytracingManager;

	private:
		ComPtr<ID3D12Device5> m_device;
	};
}