#pragma once
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

namespace Ideal
{
	class IdealRenderer;
	class D3D12Texture;
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

		void Create(std::shared_ptr<Ideal::IdealRenderer> Renderer);
		// Ω¶¿Ã¥ı∂˚ πŸ¿ŒµÂ∏¶ «—¥Ÿ.
		void BindToShader(std::shared_ptr<Ideal::IdealRenderer> Renderer);

		void SetDiffuseTextureFile(std::wstring& File)	{ m_diffuseTextureFile = File; }
		void SetSpecularTextureFile(std::wstring& File) { m_specularTextureFile = File; }
		void SetEmissiveTextureFile(std::wstring& File) { m_emissiveTextureFile = File; }
		void SetNormalTextureFile(std::wstring& File)	{ m_normalTextureFile = File; }

	private:
		Color m_ambient;
		Color m_diffuse;
		Color m_specular;
		Color m_emissive;

		std::wstring m_diffuseTextureFile;
		std::wstring m_specularTextureFile;
		std::wstring m_emissiveTextureFile;
		std::wstring m_normalTextureFile;

		std::shared_ptr<Ideal::D3D12Texture> m_diffuseTexture;
		std::shared_ptr<Ideal::D3D12Texture> m_specularTexture;
		std::shared_ptr<Ideal::D3D12Texture> m_emissiveTexture;
		std::shared_ptr<Ideal::D3D12Texture> m_normalTexture;

	private:
		CB_Material m_cbMaterialData;
	};
}