#pragma once
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <chrono>

namespace D3D12Utils {
	const uint8_t g_NumFrames = 3;

	static bool checkTearingSupport()
	{
		BOOL allowTearing = FALSE;
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory4;
		if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4)))) {
			Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;

			if (SUCCEEDED(factory4.As(&factory5))) {
				if (FAILED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
				{
					allowTearing = FALSE;
				}
			}
		}
		return allowTearing == TRUE;
	}
	//Swap Chain Helper Function
	static Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd, Microsoft::WRL::ComPtr<IDXGIFactory4> factory, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount)
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgiSwapChain4;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_dxgiSwapChain1;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc = { 1,0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = bufferCount;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		swapChainDesc.Flags = checkTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		HRESULT hr = factory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &m_dxgiSwapChain1);
		factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
		m_dxgiSwapChain1.As(&m_dxgiSwapChain4);

		return m_dxgiSwapChain4;
	}



	static Microsoft::WRL::ComPtr<ID3D12Fence> CreateFence(Microsoft::WRL::ComPtr<ID3D12Device2> device)
	{
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;

		device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));

		return m_Fence;
	}

	static HANDLE CreateEventHandle()
	{
		HANDLE fenceEvent;

		fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

		assert(fenceEvent && "Failed to create fence event");

		return fenceEvent;
	}

	static uint64_t Signal(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t& fenceValue)
	{
		uint64_t fenceValueForSignal = fenceValue++;

		commandQueue->Signal(fence.Get(), fenceValueForSignal);

		return fenceValueForSignal;
	}

	static void WaitForFenceValue(Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration = (std::chrono::milliseconds::max)())
	{
		if (fence->GetCompletedValue() < fenceValue)
		{
			fence->SetEventOnCompletion(fenceValue, fenceEvent);

			::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
		}
	}

	static void Flush(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, HANDLE fenceEvent)
	{
		uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);

		WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
	}


	static Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

		device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_commandAllocator));

		return m_commandAllocator;
	}

	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(Microsoft::WRL::ComPtr<ID3D12Device2> device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type)
	{
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_commandList;

		device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
		//m_commandList->Close();

		return m_commandList;
	}

	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool isShaderVisible)
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};

		desc.NumDescriptors = numDescriptors;
		desc.Type = type;

		if (isShaderVisible && (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER))
		{
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		}

		if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV || type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		{
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		}

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
		
		return descriptorHeap;
	}

	static inline void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			resource.Get(),
			beforeState, afterState);

		commandList->ResourceBarrier(1, &barrier);
	}
}
