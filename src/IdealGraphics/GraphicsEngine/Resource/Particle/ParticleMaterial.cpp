#include "GraphicsEngine/Resource/Particle/ParticleMaterial.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/D3D12/D3D12Shader.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/Resource/ShaderManager.h"

void Ideal::ParticleMaterial::SetShader(std::shared_ptr<Ideal::IShader> Shader)
{
	m_shader = std::static_pointer_cast<Ideal::D3D12Shader>(Shader);
}

void Ideal::ParticleMaterial::SetShaderByPath(const std::wstring& FilePath)
{
	std::shared_ptr<Ideal::ShaderManager> shaderManager = std::make_shared<Ideal::ShaderManager>();
	shaderManager->Init();
	shaderManager->LoadShaderFile(FilePath, m_shader);
}

void Ideal::ParticleMaterial::SetTexture(std::shared_ptr<Ideal::ITexture> Texture)
{
	m_texture = std::static_pointer_cast<Ideal::D3D12Texture>(Texture);
}

std::shared_ptr<Ideal::D3D12Shader> Ideal::ParticleMaterial::GetShader()
{
	return m_shader;
}

std::weak_ptr<Ideal::D3D12Texture> Ideal::ParticleMaterial::GetTexture()
{
	return m_texture;
}
