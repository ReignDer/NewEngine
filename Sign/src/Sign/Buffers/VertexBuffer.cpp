#include "VertexBuffer.h"
#include "Sign/D3D12Utils.h"
#include "Sign/Renderer/Renderer.h"
namespace Sign {
	VertexBuffer::VertexBuffer(const void* vertices, uint32_t size) : m_NumElem(size)
	{
		m_Size = m_NumElem * sizeof(VertexPosColor);
		D3D12Utils::UpdateBufferResource(
			Renderer::s_Context->GetDevice(),
			Renderer::s_CommandList,
			&m_VertexBuffer,
			&m_intermediateBuffer,
			m_NumElem,
			sizeof(VertexPosColor),
			vertices);

		D3D12Utils::TransitionResource(
			Renderer::s_CommandList,
			m_VertexBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = m_Size;
		m_VertexBufferView.StrideInBytes = sizeof(VertexPosColor);
	}
	void VertexBuffer::Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	}
	void VertexBuffer::UnBind()
	{
	}
}