#include "D3D12Context.h"

namespace Sign {
	D3D12Context::D3D12Context(HWND windowHandle, uint32_t width, uint32_t height) : m_WindowHandle(windowHandle), m_Width(width), m_Height(height)
	{
	}

	void D3D12Context::Init()
	{
		CreateDevice();
		CreateSwapChain();
		CreateRTVHeap();
		CreateFence();

		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	}

	void D3D12Context::SwapBuffers()
	{
		UINT syncInterval = m_VSync ? 1 : 0;
		UINT presentFlags = m_TearingSupported && !m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;

		m_SwapChain->Present(syncInterval, presentFlags);
		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}

	void D3D12Context::CreateDevice()
	{
		auto adapter = GetAdapter(false);
		HRESULT res = 0;
		res = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));

		if (FAILED(res))
			assert("Failed to Create Device");

		if (m_Device)
		{
			//Drawing Rasterization
			m_DirectCommandQueue = std::make_shared<D3D12CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
			//Heavy GPU workload
			m_ComputeCommandQueue = std::make_shared<D3D12CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
			//Data transfer from CPU to GPU
			m_CopyCommandQueue = std::make_shared<D3D12CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_COPY);
		}
	}

	void D3D12Context::CreateSwapChain()
	{
		UINT createFactoryFlags = 0;
		CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&m_Factory4));

		m_SwapChain = D3D12Utils::CreateSwapChain(m_WindowHandle, m_Factory4, GetCommandQueue()->GetCommandQueue(), m_Width, m_Height, D3D12Utils::g_NumFrames);
	}

	void D3D12Context::CreateRTVHeap()
	{
		m_RTVHeap = D3D12Utils::CreateDescriptorHeap(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12Utils::g_NumFrames, false);
		m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart());

		for (int i = 0; i < D3D12Utils::g_NumFrames; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));

			m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
			m_BackBuffers[i] = backBuffer;

			rtvHandle.Offset(m_RTVDescriptorSize);
		}
	}

	void D3D12Context::CreateFence()
	{
		m_Fence = D3D12Utils::CreateFence(m_Device);
		m_FenceEvent = D3D12Utils::CreateEventHandle();
	}

	void D3D12Context::FlushCommandQueue()
	{
		m_DirectCommandQueue->Flush();
		m_ComputeCommandQueue->Flush();
		m_CopyCommandQueue->Flush();
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter4> D3D12Context::GetAdapter(bool useWarp)
	{
		Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;

		UINT createFactoryFlags = 0;

		CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory));

		Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;


		Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter4;


		if (useWarp)
		{
			dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1));

			dxgiAdapter1.As(&dxgiAdapter4);

		}
		else
		{

			SIZE_T maxDedicatedVideoMemory = 0;

			for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{

				DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;

				dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);


				if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&

					SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),

						D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&

					dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
				{
					maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;

					dxgiAdapter1.As(&dxgiAdapter4);
				}
			}
		}
		return dxgiAdapter4;
	}

	std::shared_ptr<D3D12CommandQueue> D3D12Context::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
	{
		std::shared_ptr<D3D12CommandQueue> commandQueue;
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			commandQueue = m_DirectCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			commandQueue = m_ComputeCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			commandQueue = m_CopyCommandQueue;
			break;
		default:
			assert(false && "Invalid command queue type.");
		}

		return commandQueue;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12Context::GetCurrentTargetView()
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), GetCurrentBackBuffer(), m_RTVDescriptorSize);
	}

	void D3D12Context::SetFrameFenceValues(uint64_t fenceValue)
	{
		m_FrameFenceValues[m_CurrentBackBufferIndex] = fenceValue;
	}
}
