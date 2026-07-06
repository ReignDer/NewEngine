#include "D3D12CommandQueue.h"

D3D12CommandQueue::D3D12CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) 
	: m_FenceValue(0)
	, m_CommandListType(type)
	, m_d3d12Device(device)
{
	CreateCommandQueue(type);
}

void D3D12CommandQueue::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type)
{
	D3D12_COMMAND_QUEUE_DESC desc = {};

	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	m_d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12CommandQueue));
	m_d3d12Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12Fence));

	m_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(m_FenceEvent && "Failed to create fence event handle.");
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> D3D12CommandQueue::GetCommandList()
{
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList;

	if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(m_CommandAllocatorQueue.front().fenceValue))
	{
		commandAllocator = m_CommandAllocatorQueue.front().commandAllocator;
		m_CommandAllocatorQueue.pop();

		commandAllocator->Reset();

	}
	else
	{
		commandAllocator = CreateCommandAllocator();
	}
	if (!m_CommandListQueue.empty())
	{
		commandList = m_CommandListQueue.front();
		m_CommandListQueue.pop();

		commandList->Reset(commandAllocator.Get(), nullptr);

	}
	else
	{
		commandList = CreateCommandList(commandAllocator);
	}

	commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get());


	return commandList;
}

uint64_t D3D12CommandQueue::ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	commandList->Close();

	ID3D12CommandAllocator* commandAllocator;
	UINT dataSize = sizeof(commandAllocator);
	commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator);

	ID3D12CommandList* const ppCommandLists[] = {
		commandList.Get()
	};

	m_d3d12CommandQueue->ExecuteCommandLists(1, ppCommandLists);
	uint64_t fenceValue = Signal();

	m_CommandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });
	m_CommandListQueue.push(commandList);
	commandAllocator->Release();

	return fenceValue;
}

uint64_t D3D12CommandQueue::Signal()
{
	return D3D12Utils::Signal(m_d3d12CommandQueue, m_d3d12Fence, m_FenceValue);
}

bool D3D12CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	return m_d3d12Fence->GetCompletedValue() >= fenceValue;
}

void D3D12CommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
	D3D12Utils::WaitForFenceValue(m_d3d12Fence, fenceValue, m_FenceEvent);
}

void D3D12CommandQueue::Flush()
{
	D3D12Utils::Flush(m_d3d12CommandQueue, m_d3d12Fence, m_FenceValue, m_FenceEvent);
}

D3D12CommandQueue::~D3D12CommandQueue()
{
	Flush();
	if (m_FenceEvent) {
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;
	}

	while (!m_CommandAllocatorQueue.empty()) {
		m_CommandAllocatorQueue.pop();
	}
	while (!m_CommandListQueue.empty()) {
		m_CommandListQueue.pop();
	}
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> D3D12CommandQueue::CreateCommandAllocator()
{
	return D3D12Utils::CreateCommandAllocator(m_d3d12Device, m_CommandListType);
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> D3D12CommandQueue::CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
{
	return D3D12Utils::CreateCommandList(m_d3d12Device, allocator, m_CommandListType);
}
