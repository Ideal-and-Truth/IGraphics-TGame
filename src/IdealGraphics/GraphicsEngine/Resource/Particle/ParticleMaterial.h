#pragma once
#include <memory>
namespace Ideal
{
	class D3D12Shader;
	class D3D12Texture;
}

namespace Ideal
{
	class ParticleMaterial
	{
	public:
		ParticleMaterial() {};
		virtual ~ParticleMaterial() {};

	public:
		// �ۿ��� �ؾ� �� ��?
		// Custom Data �߰�
		void AddCustomData();

	private:


	private:
		std::shared_ptr<Ideal::D3D12Shader> Shader;	// ���̴� �ý���
		std::shared_ptr<Ideal::D3D12Texture> Texture; 
	};
}