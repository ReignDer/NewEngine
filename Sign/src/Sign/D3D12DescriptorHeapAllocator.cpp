#include "signpch.h"
#include "D3D12DescriptorHeapAllocator.h"
#include "Sign/Renderer/Renderer.h"
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
		m_DeferredFreeSlots.clear();
		for (int i = (int)desc.NumDescriptors - 1; i >= (int)reservedSlot; i--)
			m_FreeIndices.push_back(i);
	}
	void D3D12DescriptorHeapAllocator::Destroy()
	{
		m_Heap = nullptr;
		m_FreeIndices.clear();
	}
	void D3D12DescriptorHeapAllocator::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
	{
		std::println("Alloc");
		ProcessDeferredFrees();

		if (m_FreeIndices.empty()) {
			return;
		}

		int idx = m_FreeIndices.back();
		m_FreeIndices.pop_back();
		out_cpu_desc_handle->ptr = m_HeapStartCpu.ptr + (idx * m_HeapHandleIncrement);
		out_gpu_desc_handle->ptr = m_HeapStartGpu.ptr + (idx * m_HeapHandleIncrement);
	}
	void D3D12DescriptorHeapAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle, uint64_t currentFenceValue)
	{
		if (out_cpu_desc_handle.ptr == 0 || out_gpu_desc_handle.ptr == 0)
			return;
		std::println("Free");

		int cpu_idx = (int)((out_cpu_desc_handle.ptr - m_HeapStartCpu.ptr) / m_HeapHandleIncrement);
		int gpu_idx = (int)((out_gpu_desc_handle.ptr - m_HeapStartGpu.ptr) / m_HeapHandleIncrement);
		auto context = Renderer::GetContext();
		m_DeferredFreeSlots.push_back({currentFenceValue, cpu_idx});
	}
	void D3D12DescriptorHeapAllocator::ProcessDeferredFrees()
	{
		auto context = Renderer::GetContext();
		auto commandQueue = context->GetCommandQueue();


		uint64_t completedFenceValue = commandQueue->GetCompletedFenceValue();

		for (auto it = m_DeferredFreeSlots.begin(); it != m_DeferredFreeSlots.end();) {
			if (completedFenceValue >= it->fenceValue) {
				m_FreeIndices.push_back(it->Index);
				it = m_DeferredFreeSlots.erase(it);
			}
			else {
				it++;
			}
		}
	}
}