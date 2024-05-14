#pragma once
#include "Core/Core.h"
#include "IRenderScene.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"

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
// CB
struct CB_Global;

// ID3D12
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
		// Ver2 : 2024.05.07 : cb pool, descriptor pool을 사용하는 방식으로 바꾸겠다.
		void CreateStaticMeshPSO(std::shared_ptr<IdealRenderer> Renderer);
		void CreateSkinnedMeshPSO(std::shared_ptr<IdealRenderer> Renderer);

		// 2024.05.13 : global cb
		void CreateGlobalCB(std::shared_ptr<IdealRenderer> Renderer);
		void UpdateGlobalCBData(std::shared_ptr<IdealRenderer> Renderer);
		void SetGlobalCBDescriptorTable(std::shared_ptr<IdealRenderer> Renderer);

	private:
		//std::vector<std::shared_ptr<Ideal::IdealStaticMeshObject>> m_staticMeshObjects;
		//std::vector<std::shared_ptr<Ideal::IdealSkinnedMeshObject>> m_skinnedMeshObjects;

		std::vector<std::weak_ptr<Ideal::IdealStaticMeshObject>> m_staticMeshObjects;
		std::vector<std::weak_ptr<Ideal::IdealSkinnedMeshObject>> m_skinnedMeshObjects;

	private:
		std::shared_ptr<Ideal::D3D12PipelineStateObject> m_staticMeshPSO;
		std::shared_ptr<Ideal::D3D12PipelineStateObject> m_skinnedMeshPSO;

		ComPtr<ID3D12RootSignature> m_staticMeshRootSignature;
		ComPtr<ID3D12RootSignature> m_skinnedMeshRootSignature;

	private:
		std::shared_ptr<CB_Global> m_cbGlobal;
		D3D12DescriptorHandle m_cbGlobalHandle;
	};
}