#pragma once
#include "IMaterial.h"
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "GraphicsEngine/ConstantBufferInfo.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/Raytracing/RayTracingFlagManger.h"

namespace Ideal
{
	class IdealRenderer;
	class D3D12Texture;
	class ResourceManager;
	class IdealStaticMeshObject;
	class IdealSkinnedMeshObject;
}

namespace Ideal
{
	class IdealMaterial : public ResourceBase, public IMaterial
	{
	public:
		IdealMaterial();
		virtual ~IdealMaterial();

	public:
		//--------IMaterial Interface--------//
		virtual void SetBaseMap(std::shared_ptr<Ideal::ITexture> Texture) override;
		virtual void SetNormalMap(std::shared_ptr<Ideal::ITexture> Texture) override;
		// R : Metallic, A : Smoothness
		virtual void SetMaskMap(std::shared_ptr<Ideal::ITexture> Texture) override;
		virtual void SetEmissiveMap(std::shared_ptr<Ideal::ITexture> Texture) override;
		virtual void SetEmissiveIntensity(float Intensity) override;

		virtual std::weak_ptr<Ideal::ITexture> GetBaseMap() override;
		virtual std::weak_ptr<Ideal::ITexture> GetNomralMap() override;
		virtual std::weak_ptr<Ideal::ITexture> GetMaskMap() override;

		virtual void SetTiling(float x, float y) override;
		virtual void SetOffset(float x, float y) override;

		virtual void SetAlphaClipping(bool IsAlphBlending) override;
		virtual void SetSurfaceTypeTransparent(bool IsTransparent) override;

	public:
		void Create(std::shared_ptr<Ideal::ResourceManager> ResourceManager);
		// Ω¶¿Ã¥ı∂˚ πŸ¿ŒµÂ∏¶ «—¥Ÿ.
		void BindToShader(std::shared_ptr<Ideal::IdealRenderer> Renderer);

		void SetIsUseDiffuse(bool b) { m_cbMaterialInfo.bUseDiffuseMap = b; }
		void SetIsUseNormal(bool b) { m_cbMaterialInfo.bUseNormalMap = b; }
		void SetIsUseMetallic(bool b) { m_cbMaterialInfo.bUseMetallicMap = b; }
		void SetIsUseRoughness(bool b) { m_cbMaterialInfo.bUseRoughnessMap = b; }
		void SetIsUseEmissive(bool b) { m_cbMaterialInfo.bUseEmissiveMap = b; }

		bool GetIsUseEmissive() { return m_cbMaterialInfo.bUseEmissiveMap; }

		CB_MaterialInfo const& GetMaterialInfo() { return m_cbMaterialInfo; }
		bool GetIsAlphaClipping() { return m_isAlphaClipping; }
		bool GetIsTransmissive() { return m_cbMaterialInfo.bIsTransmissive; }

	private:
		Vector2 m_Tiling;
		Vector2 m_Offset;

		std::weak_ptr<Ideal::D3D12Texture> m_diffuseTexture;
		std::shared_ptr<Ideal::D3D12Texture> m_specularTexture;
		std::weak_ptr<Ideal::D3D12Texture> m_emissiveTexture;
		std::weak_ptr<Ideal::D3D12Texture> m_normalTexture;
		std::weak_ptr<Ideal::D3D12Texture> m_maskTexture;

	public:
		//--- Ray Tracing Info ---//
		void FreeInRayTracing();
		void CopyHandleToRayTracingDescriptorTable(ComPtr<ID3D12Device> Device);
		Ideal::D3D12DescriptorHandle GetDiffuseTextureHandleInRayTracing() { return m_diffuseTextureInRayTracing; }
		Ideal::D3D12DescriptorHandle GetNormalTextureHandleInRayTracing() { return m_normalTextureInRayTracing; }
		Ideal::D3D12DescriptorHandle GetMaskTextureHandleInRayTracing() { return m_maskTextureInRayTracing; }
		Ideal::D3D12DescriptorHandle GetEmissiveTextureHandleInRayTracing() { return m_emissiveTextureInRayTracing; }

		void SetDiffuseTextureHandleInRayTracing(Ideal::D3D12DescriptorHandle handle) { m_diffuseTextureInRayTracing = handle; }
		void SetNormalTextureHandleInRayTracing(Ideal::D3D12DescriptorHandle handle) { m_normalTextureInRayTracing = handle; }
		void SetMaskTextureHandleInRayTracing(Ideal::D3D12DescriptorHandle handle) { m_maskTextureInRayTracing = handle; }
		void SetEmissiveTextureHandleInRayTracing(Ideal::D3D12DescriptorHandle handle) { m_emissiveTextureInRayTracing = handle; }
		void AddRefCountInRayTracing() { m_refCountInRayTracing++; }

	private:
		// ray tracing descriptor table handles
		Ideal::D3D12DescriptorHandle m_diffuseTextureInRayTracing;
		Ideal::D3D12DescriptorHandle m_normalTextureInRayTracing;
		Ideal::D3D12DescriptorHandle m_maskTextureInRayTracing;
		Ideal::D3D12DescriptorHandle m_emissiveTextureInRayTracing;
		uint64 m_refCountInRayTracing = 0;

	private:
		CB_MaterialInfo	m_cbMaterialInfo;

	public:
		//void MaterialChanged(std::shared_ptr<Ideal::IdealMaterial> Material) { m_prevMaterial = Material; m_isMaterialChanged = true; }
		void TextureChanged() { m_isTextureChanged = true; Ideal::Singleton::RayTracingFlagManger::GetInstance().SetTextureChanged(); }
		bool IsTextureChanged() { return m_isTextureChanged; }

	private:
		//std::shared_ptr<Ideal::IdealMaterial> m_prevMaterial;
		//bool m_isMaterialChanged = true;
		bool m_isTextureChanged = true;
		bool m_isAlphaClipping = false;

	private:
		std::vector<std::weak_ptr<Ideal::IdealStaticMeshObject>> m_REGISTERED_staticMeshObjects;
		std::vector<std::weak_ptr<Ideal::IdealSkinnedMeshObject>> m_REGISTERED_skinnedMeshObjects;
	};
}