#pragma once
#include "directx/d3d12.h"
#include <wrl.h>
#include <cstdint>
#include <cstring>
#include "Sign/D3D12Utils.h"
namespace Sign {
	class StructuredBuffer
	{
	public:
		StructuredBuffer(uint32_t elementSize, uint32_t maxElements, uint32_t heapIndex);
		~StructuredBuffer();
		void setData(const void* data, uint32_t count);

		uint32_t GetHeapIndex() { return m_HeapIndex; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Buffer[D3D12Utils::g_NumFrames];

		uint32_t m_ElementSize;
		uint32_t m_MaxElements;
		uint32_t m_HeapIndex;

		void* m_CpuMappedAddress[D3D12Utils::g_NumFrames] = { nullptr };
	};
}

