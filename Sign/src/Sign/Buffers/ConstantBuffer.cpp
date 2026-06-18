#include "ConstantBuffer.h"
#include "Sign/Renderer/Renderer.h"
namespace Sign {
	ConstantBuffer::ConstantBuffer(uint32_t bufferSize, uint32_t heapIndex) : m_HeapIndex(heapIndex)
	{
		m_Size = (bufferSize + 255) & ~255;
		m_ConstantBuffer = D3D12Utils::CreateBuffer(Renderer::s_Context->GetDevice(),m_Size, D3D12_RESOURCE_FLAG_NONE,D3D12_RESOURCE_STATE_GENERIC_READ, D3D12Utils::UploadHeapProps);

		m_ConstantBufferView.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
		m_ConstantBufferView.SizeInBytes = m_Size;

		Renderer::s_Context->GetDevice()->CreateConstantBufferView(&m_ConstantBufferView, Renderer::s_Context->GetCPUHandleAt(m_HeapIndex));

		m_ConstantBuffer->Map(0, nullptr, &m_CpuMappedAddress);

	}
	ConstantBuffer::~ConstantBuffer()
	{
		m_ConstantBuffer->Unmap(0, nullptr);
	}
	void ConstantBuffer::setData(const void* data, uint32_t size, uint32_t offset)
	{
		std::memcpy((uint8_t*)m_CpuMappedAddress + offset, data, size);
	}
}