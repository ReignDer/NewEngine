#include "signpch.h"
#include "StructuredBuffer.h"
#include "Sign/Renderer/Renderer.h"
namespace Sign {
	StructuredBuffer::StructuredBuffer(uint32_t elementSize, uint32_t maxElements, uint32_t heapIndex)
		: m_ElementSize(elementSize), m_MaxElements(maxElements), m_HeapIndex(heapIndex)
	{
		uint32_t buffersize = m_ElementSize * m_MaxElements;
		for (int i = 0; i < D3D12Utils::g_NumFrames; i++) {
			m_Buffer[i] = D3D12Utils::CreateBuffer(Renderer::GetContext()->GetDevice(), buffersize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12Utils::UploadHeapProps);

			D3D12_SHADER_RESOURCE_VIEW_DESC srv;
			srv.Format = DXGI_FORMAT_UNKNOWN;
			srv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srv.Buffer.FirstElement = 0;
			srv.Buffer.NumElements = m_MaxElements;
			srv.Buffer.StructureByteStride = m_ElementSize;
			srv.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;



			uint32_t uniqueHeapIndex = m_HeapIndex + i;
			Renderer::GetContext()->GetDevice()->CreateShaderResourceView(m_Buffer[i].Get(), &srv, Renderer::GetContext()->GetCPUHandleAt(uniqueHeapIndex));

			m_Buffer[i]->Map(0, nullptr, &m_CpuMappedAddress[i]);
		}
	}
	StructuredBuffer::~StructuredBuffer()
	{
		for (int i = 0; i < D3D12Utils::g_NumFrames; i++) {
			if (m_Buffer[i])
				m_Buffer[i]->Unmap(0, nullptr);
		}
	}
	void StructuredBuffer::setData(const void* data, uint32_t count)
	{
		auto currentFrame = Renderer::GetContext()->GetCurrentBackBuffer();
		std::memcpy(m_CpuMappedAddress[currentFrame], data, m_ElementSize * (std::min)(count,m_MaxElements));
	}
}