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

	inline const D3D12_HEAP_PROPERTIES UploadHeapProps = {
		D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0,0
	};
	inline const D3D12_HEAP_PROPERTIES DefaultHeapProps = {
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0,0
	};
	inline const D3D12_HEAP_PROPERTIES ReadBackHeapProps = {
		D3D12_HEAP_TYPE_READBACK, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0,0
	};

	inline bool checkTearingSupport()
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
	inline Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd, Microsoft::WRL::ComPtr<IDXGIFactory4> factory, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount)
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



	inline Microsoft::WRL::ComPtr<ID3D12Fence> CreateFence(Microsoft::WRL::ComPtr<ID3D12Device2> device)
	{
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;

		device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));

		return m_Fence;
	}

	inline HANDLE CreateEventHandle()
	{
		HANDLE fenceEvent;

		fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

		assert(fenceEvent && "Failed to create fence event");

		return fenceEvent;
	}

	inline uint64_t Signal(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t& fenceValue)
	{
		uint64_t fenceValueForSignal = fenceValue++;

		commandQueue->Signal(fence.Get(), fenceValueForSignal);

		return fenceValueForSignal;
	}

	inline void WaitForFenceValue(Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration = (std::chrono::milliseconds::max)())
	{
		if (fence->GetCompletedValue() < fenceValue)
		{
			fence->SetEventOnCompletion(fenceValue, fenceEvent);

			::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
		}
	}

	inline void Flush(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, HANDLE fenceEvent)
	{
		uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);

		WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
	}


	inline Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

		device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_commandAllocator));

		return m_commandAllocator;
	}

	inline Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(Microsoft::WRL::ComPtr<ID3D12Device2> device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type)
	{
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_commandList;

		device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
		//m_commandList->Close();

		return m_commandList;
	}

	inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device2* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool isShaderVisible)
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

	inline void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			resource.Get(),
			beforeState, afterState);

		commandList->ResourceBarrier(1, &barrier);
	}

	inline void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device2> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
		ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource,
		size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
	{
		size_t bufferSize = numElements * elementSize;


		auto heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDescription = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
		//Create Destination Resource
		device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(pDestinationResource));

		//Create Intermediate Resource
		if (bufferData)
		{
			auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
			device->CreateCommittedResource(
				&uploadHeap,
				D3D12_HEAP_FLAG_NONE,
				&uploadBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(pIntermediateResource));

			//Transfer CPU buffer to GPU resources

			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = bufferData;
			subResourceData.RowPitch = bufferSize;
			subResourceData.SlicePitch = subResourceData.RowPitch;

			UpdateSubresources(commandList.Get(), *pDestinationResource, *pIntermediateResource, 0, 0, 1, &subResourceData);
		}
	}
	inline void UpdateTextureBufferResource(Microsoft::WRL::ComPtr<ID3D12Device2> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
		ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource,
		size_t numElements, size_t elementSize, const void* bufferData, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
	{
		size_t bufferSize = numElements * elementSize;


		auto heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDescription = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
		//Create Destination Resource
		device->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(pDestinationResource));

		//Create Intermediate Resource
		if (bufferData)
		{
			auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
			device->CreateCommittedResource(
				&uploadHeap,
				D3D12_HEAP_FLAG_NONE,
				&uploadBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(pIntermediateResource));

			//Transfer CPU buffer to GPU resources

			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = bufferData;
			subResourceData.RowPitch = bufferSize;
			subResourceData.SlicePitch = subResourceData.RowPitch;

			UpdateSubresources(commandList.Get(), *pDestinationResource, *pIntermediateResource, 0, 0, 1, &subResourceData);
		}
	}
	inline Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(Microsoft::WRL::ComPtr<ID3D12Device2> device
		,uint64_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState, const D3D12_HEAP_PROPERTIES& heapProps )
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;
		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Alignment = 0;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Flags = flags;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.Height = 1;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.MipLevels = 1;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Width = size;

		device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, initState, nullptr, IID_PPV_ARGS(&bufferResource));

		return bufferResource;
	}
	inline Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(ID3D12Device2* device
		, uint64_t width, uint64_t height, D3D12_RESOURCE_DIMENSION dimension, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState, const D3D12_HEAP_PROPERTIES& heapProps, D3D12_CLEAR_VALUE* clearValue = nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;
		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Alignment = 0;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.Dimension = dimension;
		bufferDesc.Flags = flags;
		bufferDesc.Format = format;
		bufferDesc.Height = height;
		bufferDesc.Layout = (dimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN;
		bufferDesc.MipLevels = 1;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Width = width;

		device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, initState, clearValue, IID_PPV_ARGS(&bufferResource));

		return bufferResource;
	}
	inline void CreateShaderResourceView(ID3D12Device2* device, ID3D12Resource* resource, DXGI_FORMAT internalFormat, D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandle) {
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = internalFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = resource->GetDesc().MipLevels;

		device->CreateShaderResourceView(resource, &srvDesc, *cpuHandle);
	}
}
