#include "Mesh.h"

namespace Sign {
	Mesh::Mesh(const void* vertices, uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount)
	{
		m_VertexArray = std::make_shared<VertexArray>();

		std::shared_ptr<Sign::VertexBuffer> VB;
		std::shared_ptr<Sign::IndexBuffer>  IB;

		VB = std::make_shared<Sign::VertexBuffer>(vertices, vertexCount);
		m_VertexArray->AddVertexBuffers(VB);
		IB = std::make_shared<Sign::IndexBuffer>(indices, indexCount);
		m_VertexArray->SetIndexBuffer(IB);
	}
	
}