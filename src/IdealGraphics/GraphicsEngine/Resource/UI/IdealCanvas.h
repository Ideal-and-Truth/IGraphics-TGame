#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"

struct ID3D12Device;
struct ID3D12RootSignature;

namespace Ideal
{
	class D3D12PipelineStateObject;
	class IdealSprite;
}

namespace Ideal
{
	struct UILayer
	{
		int32 depth;
		std::string name;
		UILayer(const std::string& n, int32 d) : depth(d), name(n) {}

		static bool compareLayers(const UILayer& layer1, const UILayer& layer2)
		{
			return layer1.depth < layer2.depth;
		}

		std::vector<std::weak_ptr<Ideal::IdealSprite>> m_sprites;
	};

	/// <summary>
	/// 2024.08.05 
	/// UI를 관리하기 위한 클래스
	/// 여기에 이미지든 텍스쳐든 들어갈 예정이다.
	/// </summary>
	class IdealCanvas
	{
	public:
		IdealCanvas();
		virtual ~IdealCanvas();

	public:
		void Init(ComPtr<ID3D12Device> Device);
		void DrawCanvas();

		void AddSprite(std::weak_ptr<Ideal::IdealSprite> Sprite);
		void DeleteSprite(std::weak_ptr<Ideal::IdealSprite> Sprite);

	private:
		void CreateRootSignature(ComPtr<ID3D12Device> Device);
		void CreatePSO(ComPtr<ID3D12Device> Device);

		ComPtr<ID3D12RootSignature> m_rectRootSignature;
		std::shared_ptr<Ideal::D3D12PipelineStateObject> m_rectPSO;

	private:
		std::vector<std::weak_ptr<Ideal::IdealSprite>> m_sprites;
	};
}