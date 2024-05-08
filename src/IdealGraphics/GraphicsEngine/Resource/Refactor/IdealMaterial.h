#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
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
		// 쉐이더랑 바인드를 한다.
		void BindToShader(std::shared_ptr<Ideal::IdealRenderer> Renderer);
		void BindToShader2(std::shared_ptr<Ideal::IdealRenderer> Renderer);

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
		// TEMP : 2024.05.03 잘 변하지 않는 데이터긴 하지만 일단 만들어만 둘 예정
		Ideal::D3D12ConstantBuffer m_cbMaterial;
		CB_Material m_cbMaterialData;
	};
}