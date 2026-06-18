#pragma once
#include <DirectXMath.h>
#include <cstdint>
#include "Sign/Buffers/VertexArray.h"

namespace Sign {
	class Mesh
	{
	public:
		Mesh(const void* vertices, uint32_t vertexCount, const WORD* indices, uint32_t indexCount, 
			const DirectX::XMFLOAT3& translation,
			const DirectX::XMFLOAT3& scale,
			const DirectX::XMFLOAT4& rotation);
		~Mesh() = default;

		const DirectX::XMMATRIX& GetTransform() const { return m_Transform; }
		const DirectX::XMFLOAT3& GetTranslation() const { return m_Translation; }
		const DirectX::XMFLOAT3& GetScale() const { return m_Scale; }
		const DirectX::XMFLOAT4& GetRotation() const { return m_Rotation; }
		const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }


		void SetTranslation(const DirectX::XMFLOAT3& pos) { m_Translation = pos; RecalculateTransform(); }
		void SetScale(const DirectX::XMFLOAT3& scale) { m_Scale = scale; RecalculateTransform(); };
		void SetRotation(const DirectX::XMFLOAT4& rotation) { m_Rotation = rotation; RecalculateTransform(); };
	private:
		void RecalculateTransform();
	private:
		DirectX::XMFLOAT3 m_Translation;
		DirectX::XMFLOAT3 m_Scale;
		DirectX::XMFLOAT4 m_Rotation;
		DirectX::XMMATRIX m_Transform;

		std::shared_ptr<VertexArray> m_VertexArray;
	};
}

