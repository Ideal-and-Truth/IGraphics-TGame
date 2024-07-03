#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include <d3d12.h>
#include <d3dx12.h>

namespace Ideal
{
	struct DeferredDeleteResource
	{
		ComPtr<ID3D12Resource> Resource;
		uint32 ContextIndex;
	};

	class DeferredDeleteManager
	{
		static const uint32 MAX_PENDING_FRAMES = G_SWAP_CHAIN_NUM - 1;

	public:
		DeferredDeleteManager();
		~DeferredDeleteManager();

	public:
		void DeleteDeferredResources(uint32 CurrentContextIndex);
		void AddResourceToDelete(ComPtr<ID3D12Resource> Resource);

		std::vector<ComPtr<ID3D12Resource>> m_resourcesToDelete[MAX_PENDING_FRAMES];
		uint32 m_currentContextIndex = 0;
	};
}