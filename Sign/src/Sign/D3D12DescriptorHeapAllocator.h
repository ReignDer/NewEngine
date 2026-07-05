#pragma once
#include <directx/d3d12.h>
#include <vector>
namespace Sign {
	class D3D12DescriptorHeapAllocator
	{
	public:
		void Create(ID3D12Device2* device, ID3D12DescriptorHeap* heap, UINT reservedSlot = 0);
		void Destroy();
		void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle);
		void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle);

	private:
		ID3D12DescriptorHeap*		m_Heap = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE	m_HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
		D3D12_CPU_DESCRIPTOR_HANDLE m_HeapStartCpu;
		D3D12_GPU_DESCRIPTOR_HANDLE m_HeapStartGpu;
		UINT						m_HeapHandleIncrement;
		std::vector<int>			m_FreeIndices;
	};
}

