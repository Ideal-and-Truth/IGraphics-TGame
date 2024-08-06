#pragma once
#include "Core/Core.h"

struct ID3D12Device;
struct ID3D12RootSignature;

namespace Ideal
{
	class D3D12PipelineStateObject;
}

namespace Ideal
{
	struct UILayer
	{
		int32 depth;
		std::string name;
		UILayer(uint32 d, const std::string& n) : depth(d), name(n) {}

		static bool compareLayers(const UILayer& layer1, const UILayer& layer2)
		{
			return layer1.depth < layer2.depth;
		}
	};

	namespace RectRootSignature
	{
		namespace Slot
		{
			enum Enum
			{
				SRV_Sprite = 0,
				CBV_RectInfo,
				Count
			};
		}
	}

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
		void SortLayers();
		
	private:
		void CreateRootSignature(ComPtr<ID3D12Device> Device);
		void CreatePSO(ComPtr<ID3D12Device> Device);

		ComPtr<ID3D12RootSignature> m_rectRootSignature;
		std::shared_ptr<Ideal::D3D12PipelineStateObject> m_rectPSO;

	private:
		std::vector<Ideal::UILayer> m_layers;
	};
}