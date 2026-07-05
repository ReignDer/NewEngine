#pragma once
#include <Windows.h>
#include <memory>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include "D3D12Utils.h"
#include "D3D12CommandQueue.h"
#include "D3D12DescriptorHeapAllocator.h"
namespace Sign {
	class D3D12Context
	{
	public:
		D3D12Context(HWND windowHandle, uint32_t width, uint32_t height);

		void Init();
		void SwapBuffers();


		Microsoft::WRL::ComPtr<IDXGIAdapter4>			GetAdapter(bool useWarp);

		std::shared_ptr<D3D12CommandQueue>				GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
		Microsoft::WRL::ComPtr<ID3D12Device2>			GetDevice() { return m_Device; }

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	GetRTVDescriptorHeap() { return m_RTVHeap; }
		UINT											GetRTVDescriptorSize() { return m_RTVDescriptorSize; }
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	Get_CBV_SRV_UAV_DescriptorHeap() { return m_CBV_SRV_UAV_Heap; }
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	GetDSVDescriptorHeap() { return m_DSVHeap; }
		D3D12DescriptorHeapAllocator&					Get_CBV_SRV_UAV_Allocator() { return m_CBV_SRV_UAV_Allocator; }

		Microsoft::WRL::ComPtr<ID3D12Fence>				GetFence() { return m_Fence; }
		HANDLE											GetFenceEvent() { return m_FenceEvent; }
		Microsoft::WRL::ComPtr<IDXGISwapChain4>			GetSwapChain() { return m_SwapChain; }
		UINT											GetCurrentBackBuffer() { return m_SwapChain->GetCurrentBackBufferIndex(); }
		CD3DX12_CPU_DESCRIPTOR_HANDLE					GetCurrentTargetView();
		Microsoft::WRL::ComPtr<ID3D12Resource>			GetBackBuffer(uint32_t index) const { return m_BackBuffers[index]; }

		//CANNOT BE USED FOR SAMPLERS e.g. RTV, DSV
		CD3DX12_CPU_DESCRIPTOR_HANDLE					GetCPUHandleAt(unsigned int index);
		CD3DX12_GPU_DESCRIPTOR_HANDLE					GetGPUHandleAt(unsigned int index);

		uint32_t										GetWidth() const { return m_Width; }
		uint32_t										GetHeight() const { return m_Height; }

		void ResizeDepthBuffer(int width, int height);
		void ResizeSwapBuffer(uint32_t width, uint32_t height);

		void SetFrameFenceValues(uint64_t fenceValue);

		void FlushCommandQueue();
		void UpdateRenderTargetViews();

	private:

		void CreateDevice();
		void CreateSwapChain();
		void CreateRTVHeap();
		void CreateCBV_SRV_UAV_Heap();
		void CreateDSVHeap();
		void CreateFence();

	private:
		HWND m_WindowHandle;
		bool m_VSync = true;
		bool m_TearingSupported = false;
		unsigned int m_RTVDescriptorSize;
		unsigned int m_CBV_SRV_UAV_DescriptorSize;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory4;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CBV_SRV_UAV_Heap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
		D3D12DescriptorHeapAllocator m_CBV_SRV_UAV_Allocator;

		std::shared_ptr<D3D12CommandQueue> m_DirectCommandQueue;
		std::shared_ptr<D3D12CommandQueue> m_ComputeCommandQueue;
		std::shared_ptr<D3D12CommandQueue> m_CopyCommandQueue;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		uint64_t m_FenceValue = 0;
		uint64_t m_FrameFenceValues[D3D12Utils::g_NumFrames] = {};
		HANDLE m_FenceEvent;


		Microsoft::WRL::ComPtr<ID3D12Resource> m_BackBuffers[D3D12Utils::g_NumFrames];
		Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthBuffer;

		UINT m_CurrentBackBufferIndex;

		uint32_t m_Width, m_Height;

	};
}

