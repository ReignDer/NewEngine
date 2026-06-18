#pragma once
#include "directx/d3d12.h"
#include <wrl.h>
#include <cstdint>
#include <cstring>
#include "Sign/D3D12Utils.h"

namespace Sign {
	class ConstantBuffer
	{
	public:
		ConstantBuffer(uint32_t bufferSize, uint32_t heapIndex);
		~ConstantBuffer();
		void setData(const void* data, uint32_t size, uint32_t offset = 0);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBuffer;
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_ConstantBufferView;

		uint32_t m_Size;
		uint32_t m_HeapIndex;
		
		void* m_CpuMappedAddress = nullptr;
	};
}

