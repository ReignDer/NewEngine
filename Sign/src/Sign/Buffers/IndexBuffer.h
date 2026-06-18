#pragma once
#include <directx/d3d12.h>
#include <wrl.h>
#include <cstdint>



namespace Sign {
	class IndexBuffer
	{
	public:
		IndexBuffer(const WORD* indices, uint32_t size);

		void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);
		void UnBind();
		size_t GetCount() const { return m_NumElem; }
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateIndexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		uint32_t m_Size;
		size_t m_NumElem;
	};
}

