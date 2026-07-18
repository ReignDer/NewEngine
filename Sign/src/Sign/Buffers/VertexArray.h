#pragma once
#include <vector>
#include <memory>
#include <directx/d3d12.h>
#include <wrl.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Sign {
	class VertexArray
	{
	public:
		VertexArray();
		void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) const;
		void UnBind() const;
		void AddVertexBuffers(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
		size_t GetIndexBufferCount() const;
		std::shared_ptr<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffers[0]; }


	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
}

