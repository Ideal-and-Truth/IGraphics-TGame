#pragma once
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"

namespace Ideal
{
	template <typename T>
	class D3D12StructuredBuffer : public D3D12UploadBuffer
	{
	public:
		static_assert(sizeof(T) & 16 == 0, "StructuredBuffer Align Size");
		D3D12StructuredBuffer() : m_mappedBuffers(nullptr), m_numInstances(0) {}

		void Create(ID3D12Device* Device, uint32 NumElements, uint32 NumInstances = 1, const wchar_t* Name = nullptr)
		{
			m_numInstances = NumInstances;
			m_staging.resize(NumElements);
			uint32 bufferSize = NumInstances * NumElements * sizeof(T);
			D3D12UploadBuffer::Create(Device, bufferSize);
			m_mappedBuffers = reinterpret_cast<T*>(D3D12UploadBuffer::MapCpuWriteOnly());
		}

		void CopyStagingToGPU(uint32 instanceIndex = 0)
		{
			uint32 NumElements = m_staging.size();
			uint32 ElementSize = sizeof(T);
			memcpy(m_mappedBuffers + instanceIndex * NumElements, &m_staging[0], NumElements * ElementSize);
		}

	private:
		T* m_mappedBuffers;
		uint32 m_numInstances;
		std::vector<T> m_staging;
	};
}