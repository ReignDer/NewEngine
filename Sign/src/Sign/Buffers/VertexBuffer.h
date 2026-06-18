#pragma once
#include <directx/d3d12.h>
#include <wrl.h>
#include <cstdint>

#include "Sign/Renderer/MeshUtils.h"


namespace Sign {
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* vertices, uint32_t size);

		void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);
		void UnBind();

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_intermediateBuffer;

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

		uint32_t m_Size;
		size_t m_NumElem;

	};
}

