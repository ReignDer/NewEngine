#include "signpch.h"
#include "D3D12DescriptorHeapAllocator.h"

namespace Sign {
	void D3D12DescriptorHeapAllocator::Create(ID3D12Device2* device, ID3D12DescriptorHeap* heap, UINT reservedSlot)
	{
		m_Heap = heap;
		D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
		m_HeapType = desc.Type;
		m_HeapStartCpu = m_Heap->GetCPUDescriptorHandleForHeapStart();
		m_HeapStartGpu = m_Heap->GetGPUDescriptorHandleForHeapStart();
		m_HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(m_HeapType);
		m_FreeIndices.reserve((int)desc.NumDescriptors);
		for (int i = (int)desc.NumDescriptors; i >= (int)reservedSlot; i--)
			m_FreeIndices.push_back(i - 1);
	}
	void D3D12DescriptorHeapAllocator::Destroy()
	{
		m_Heap = nullptr;
		m_FreeIndices.clear();
	}
	void D3D12DescriptorHeapAllocator::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
	{
		int idx = m_FreeIndices.back();
		m_FreeIndices.pop_back();
		out_cpu_desc_handle->ptr = m_HeapStartCpu.ptr + (idx * m_HeapHandleIncrement);
		out_gpu_desc_handle->ptr = m_HeapStartGpu.ptr + (idx * m_HeapHandleIncrement);
	}
	void D3D12DescriptorHeapAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
	{
		if (out_cpu_desc_handle.ptr == 0 || out_gpu_desc_handle.ptr == 0)
			return;

		int cpu_idx = (int)((out_cpu_desc_handle.ptr - m_HeapStartCpu.ptr) / m_HeapHandleIncrement);
		int gpu_idx = (int)((out_gpu_desc_handle.ptr - m_HeapStartGpu.ptr) / m_HeapHandleIncrement);
		m_FreeIndices.push_back(cpu_idx);
	}
}