#pragma once
#include <directx/d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <queue> 

#include "D3D12Utils.h"
class D3D12CommandQueue
{
public:
	D3D12CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
	void CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type);
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetCommandList();
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() { return m_d3d12CommandQueue; }

	uint64_t ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);
	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFenceValue(uint64_t fenceValue);
	void Flush();
	~D3D12CommandQueue();

protected:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);

private:
	struct CommandAllocatorEntry
	{
		uint64_t fenceValue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	};

	using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>;
	using CommandListQueue = std::queue<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>>;

	D3D12_COMMAND_LIST_TYPE     m_CommandListType;
	Microsoft::WRL::ComPtr<ID3D12Device2>       m_d3d12Device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue>  m_d3d12CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12Fence>         m_d3d12Fence;
	HANDLE                      m_FenceEvent;
	uint64_t                    m_FenceValue;

	CommandAllocatorQueue       m_CommandAllocatorQueue;
	CommandListQueue            m_CommandListQueue;
};

