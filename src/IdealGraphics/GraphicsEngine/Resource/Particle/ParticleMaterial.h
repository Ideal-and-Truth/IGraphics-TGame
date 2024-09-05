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
		// 밖에서 해야 할 것?
		// Custom Data 추가
		void AddCustomData();

	private:


	private:
		std::shared_ptr<Ideal::D3D12Shader> Shader;	// 쉐이더 시스템
		std::shared_ptr<Ideal::D3D12Texture> Texture; 
	};
}