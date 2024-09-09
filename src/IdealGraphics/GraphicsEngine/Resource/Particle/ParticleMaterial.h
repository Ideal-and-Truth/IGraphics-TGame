#pragma once
#include "IParticleMaterial.h"
#include "Core/Core.h"

struct ID3D12Device;

namespace Ideal
{
	class D3D12Shader;
	class D3D12Texture;
}

/// <summary>
/// ��� 1 
/// UV.z �� Curve�����͸� �ִ� ���
/// BitPacking �� �̿��Ͽ� 16bit float�� �� ���� �ְ� �ð��� ���� ������ ����Ѵ�.
/// �̷��� �Ǹ� Root Signature�� PSO�� �ٸ� ������� ����.. �ʤ���...
/// PSO�� ��·�� ���̴� ������ �޶����� ������ Vertex�� ������ ���ϵǰ�
/// Constant Buffer�� �� �߰����� �ʾƵ� �ȴ�.
/// 
/// �׷��ٸ� �ؾ� �� ��
/// ������ ���� UV�� float4�� �ٲٰ� UV.zw ���� �� �� �ְ� �ؾ� �Ѵ�.
/// Vertex Data
/// {
///		Position, Normal, Tangent, UV0, UV1, Color
/// }
/// 
/// �ؽ��Ŀ� �̹����� �´� UV��ǥ�� ���� �־�� �Ѵ�.
/// 
/// ���2
/// constant buffer�� �� �����ڴ´�.
/// </summary>
namespace Ideal
{
	class ParticleMaterial : public IParticleMaterial
	{
	public:
		ParticleMaterial() {};
		virtual ~ParticleMaterial() {};

	public:
		virtual void SetShader(std::shared_ptr<Ideal::IShader> Shader) override;
		virtual void SetShaderByPath(const std::wstring& FilePath) override;
		virtual void SetTexture(std::shared_ptr<Ideal::ITexture> Texture) override;

	public:
		std::shared_ptr<Ideal::D3D12Shader> GetShader();

	private:
		std::shared_ptr<Ideal::D3D12Shader> m_shader;	// ���̴� �ý���
		std::weak_ptr<Ideal::D3D12Texture> m_texture;	
	};
}