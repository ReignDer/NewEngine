#include "IndexBuffer.h"

#include "Sign/Renderer/Mesh.h"
#include "Sign/D3D12Utils.h"
#include "Sign/Renderer/Renderer.h"
namespace Sign{
	IndexBuffer::IndexBuffer(const WORD* indices, uint32_t size) : m_NumElem(size)
	{
		m_Size = m_NumElem * sizeof(WORD);
		D3D12Utils::UpdateBufferResource(
			Renderer::s_Context->GetDevice(),
			Renderer::s_CommandList,
			&m_IndexBuffer,
			&intermediateIndexBuffer,
			m_NumElem,
			sizeof(WORD),
			indices);

		D3D12Utils::TransitionResource(
			Renderer::s_CommandList,
			m_IndexBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
		m_IndexBufferView.SizeInBytes = m_Size;
	}
	void IndexBuffer::Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		commandList->IASetIndexBuffer(&m_IndexBufferView);
	}
	void IndexBuffer::UnBind()
	{
	}

}
