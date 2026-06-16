#pragma once
#include <Windows.h>
#include <memory>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include "D3D12Utils.h"
#include "D3D12CommandQueue.h"
namespace Sign {
	class D3D12Context
	{
	public:
		D3D12Context(HWND windowHandle, uint32_t width, uint32_t height);

		void Init();
		void SwapBuffers();

		void CreateDevice();
		void CreateSwapChain();
		void CreateRTVHeap();
		void CreateFence();
		void FlushCommandQueue();

		Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp);

		std::shared_ptr<D3D12CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
		Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice() { return m_Device; }
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRTVDescriptorHeap() { return m_RTVHeap; }
		UINT GetRTVDescriptorSize() { return m_RTVDescriptorSize; }
		Microsoft::WRL::ComPtr<ID3D12Fence> GetFence() { return m_Fence; }
		HANDLE GetFenceEvent() { return m_FenceEvent; }
		Microsoft::WRL::ComPtr<IDXGISwapChain4> GetSwapChain() { return m_SwapChain; }
		UINT GetCurrentBackBuffer() { return m_SwapChain->GetCurrentBackBufferIndex(); }
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentTargetView();
		Microsoft::WRL::ComPtr<ID3D12Resource> GetBackBuffer(uint32_t index) const { return m_BackBuffers[index]; }

		void SetFrameFenceValues(uint64_t fenceValue);

	private:
		HWND m_WindowHandle;
		bool m_VSync = true;
		bool m_TearingSupported = false;
		unsigned int m_RTVDescriptorSize;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory4;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;

		std::shared_ptr<D3D12CommandQueue> m_DirectCommandQueue;
		std::shared_ptr<D3D12CommandQueue> m_ComputeCommandQueue;
		std::shared_ptr<D3D12CommandQueue> m_CopyCommandQueue;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
		uint64_t m_FenceValue = 0;
		uint64_t m_FrameFenceValues[D3D12Utils::g_NumFrames] = {};
		HANDLE m_FenceEvent;


		Microsoft::WRL::ComPtr<ID3D12Resource> m_BackBuffers[D3D12Utils::g_NumFrames];

		UINT m_CurrentBackBufferIndex;

		uint32_t m_Width, m_Height;

	};
}

