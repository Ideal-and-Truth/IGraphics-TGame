#pragma once

#include "Core/Core.h"
#include <d3dcommon.h>
#include <d3dx12.h>

namespace Ideal
{
	class D3D12Shader
	{
	public:
		D3D12Shader();
		virtual ~D3D12Shader();

	public:
		void CompileFromFile(
			const std::wstring& Path,
			D3D_SHADER_MACRO* ShaderMacro,
			ID3DInclude* ShaderInclude,
			const std::string& Entry,
			const std::string& Version
		);

		const CD3DX12_SHADER_BYTECODE& GetShaderByteCode();

	private:
		ComPtr<ID3DBlob> m_shader;
		CD3DX12_SHADER_BYTECODE m_shaderByteCode;
	};
}