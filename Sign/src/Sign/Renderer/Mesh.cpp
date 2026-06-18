#include "Mesh.h"

namespace Sign {
	Mesh::Mesh(const void* vertices, uint32_t vertexCount, const WORD* indices, uint32_t indexCount,
		const DirectX::XMFLOAT3& translation,
		const DirectX::XMFLOAT3& scale,
		const DirectX::XMFLOAT4& rotation)
	{
		m_VertexArray = std::make_shared<VertexArray>();
		m_Translation = translation;
		m_Scale = scale;
		m_Rotation = rotation;
		RecalculateTransform();

		std::shared_ptr<Sign::VertexBuffer> VB;
		std::shared_ptr<Sign::IndexBuffer>  IB;

		VB = std::make_shared<Sign::VertexBuffer>(vertices, vertexCount);
		m_VertexArray->AddVertexBuffers(VB);
		IB = std::make_shared<Sign::IndexBuffer>(indices, indexCount);
		m_VertexArray->SetIndexBuffer(IB);
	}
	void Mesh::RecalculateTransform()
	{
		auto translation = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_Translation));
		auto scale = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_Scale));
		auto rotation = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_Rotation));
		m_Transform =  scale * rotation * translation;
	}
}