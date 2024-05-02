#pragma once
#include "Core/Core.h"
#include "IRenderScene.h"

namespace Ideal
{
	class IMeshObject;
	class IdealRenderer;
	class IdealStaticMeshObject;
	class IdealSkinnedMeshObject;

	// D3D12
	class D3D12PipelineStateObject;
	class D3D12Shader;
}

struct ID3D12RootSignature;

namespace Ideal
{
	class IdealRenderScene : public IRenderScene
	{
	public:
		IdealRenderScene();
		virtual ~IdealRenderScene();

	public:
		void Init(std::shared_ptr<IdealRenderer> Renderer);
		void Draw(std::shared_ptr<IdealRenderer> Renderer);

	public:
		virtual void AddObject(std::shared_ptr<Ideal::IMeshObject> MeshObject) override;

	private:
		void CreateStaticMeshPSO(std::shared_ptr<IdealRenderer> Renderer);
		void CreateSkinnedMeshPSO(std::shared_ptr<IdealRenderer> Renderer);

	private:
		std::vector<std::shared_ptr<Ideal::IdealStaticMeshObject>> m_staticMeshObjects;
		std::vector<std::shared_ptr<Ideal::IdealSkinnedMeshObject>> m_skinnedMeshObjects;

	private:
		std::shared_ptr<Ideal::D3D12PipelineStateObject> m_staticMeshPSO;
		std::shared_ptr<Ideal::D3D12PipelineStateObject> m_skinnedMeshPSO;

		ComPtr<ID3D12RootSignature> m_staticMeshRootSignature;
		ComPtr<ID3D12RootSignature> m_skinnedMeshRootSignature;
	};
}