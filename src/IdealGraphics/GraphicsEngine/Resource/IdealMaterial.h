#pragma once
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

namespace Ideal
{
	class IdealRenderer;
	class D3D12Texture;
	class ResourceManager;
}

namespace Ideal
{
	class IdealMaterial : public ResourceBase
	{
	public:
		IdealMaterial();
		virtual ~IdealMaterial();

		void SetAmbient(Color c) { m_ambient = c; }
		void SetDiffuse(Color c) { m_diffuse = c; }
		void SetSpecular(Color c) { m_specular = c; }
		void SetEmissive(Color c) { m_emissive = c; }

		void SetMetallicFactor(const float& f) { m_metallicFactor = f; }
		void SetRoughnessFactor(const float& f) { m_roughnessFactor = f; }

		void Create(std::shared_ptr<Ideal::ResourceManager> ResourceManager);
		// Ω¶¿Ã¥ı∂˚ πŸ¿ŒµÂ∏¶ «—¥Ÿ.
		void BindToShader(std::shared_ptr<Ideal::IdealRenderer> Renderer);

		void SetDiffuseTextureFile(std::wstring& File)	{ m_diffuseTextureFile = File; }
		void SetIsUseDiffuse(bool b) {m_cbMaterialInfo.bUseDiffuseMap = b; }
		void SetSpecularTextureFile(std::wstring& File) { m_specularTextureFile = File; }

		void SetEmissiveTextureFile(std::wstring& File) { m_emissiveTextureFile = File; }
		void SetNormalTextureFile(std::wstring& File)	{ m_normalTextureFile = File; }
		void SetIsUseNormal(bool b) { m_cbMaterialInfo.bUseNormalMap = b; }

		void SetMetallicTextureFile(std::wstring& File) { m_metallicTextureFile = File; }
		void SetIsUseMetallic(bool b) { m_cbMaterialInfo.bUseMetallicMap = b; }
		void SetRoughnessTextureFile(std::wstring& File) { m_roughnessTextureFile = File; }
		void SetIsUseRoughness(bool b) { m_cbMaterialInfo.bUseRoughnessMap = b; }

		std::shared_ptr<Ideal::D3D12Texture> GetDiffuseTexture() { return m_diffuseTexture; }
		std::shared_ptr<Ideal::D3D12Texture> GetSpecularTexture() { return m_specularTexture; }
		std::shared_ptr<Ideal::D3D12Texture> GetEmissiveTexture() { return m_emissiveTexture; }
		std::shared_ptr<Ideal::D3D12Texture> GetNormalTexture() { return m_normalTexture; }

		std::shared_ptr<Ideal::D3D12Texture> GetMetallicTexture() { return m_metalicTexture; }
		std::shared_ptr<Ideal::D3D12Texture> GetRoughnessTexture() { return m_roughnessTexture; }

		CB_MaterialInfo const& GetMaterialInfo() { return m_cbMaterialInfo; }

	private:
		Color m_ambient;
		Color m_diffuse;
		Color m_specular;
		Color m_emissive;

		float m_metallicFactor;
		float m_roughnessFactor;

		std::wstring m_diffuseTextureFile;
		std::wstring m_specularTextureFile;
		std::wstring m_emissiveTextureFile;
		std::wstring m_normalTextureFile;

		std::wstring m_metallicTextureFile;
		std::wstring m_roughnessTextureFile;

		std::shared_ptr<Ideal::D3D12Texture> m_diffuseTexture;
		std::shared_ptr<Ideal::D3D12Texture> m_specularTexture;
		std::shared_ptr<Ideal::D3D12Texture> m_emissiveTexture;
		std::shared_ptr<Ideal::D3D12Texture> m_normalTexture;
		
		std::shared_ptr<Ideal::D3D12Texture> m_metalicTexture;
		std::shared_ptr<Ideal::D3D12Texture> m_roughnessTexture;

	private:
		CB_Material m_cbMaterialData;
		CB_MaterialInfo	m_cbMaterialInfo;
	};
}