#pragma once
#include <cstdint>
#include "Sign/Buffers/VertexArray.h"
#include "Sign/Math/SignMath.h"

namespace Sign {
	class Mesh
	{
	public:
		Mesh(const void* vertices, uint32_t vertexCount, const WORD* indices, uint32_t indexCount);
		~Mesh() = default;
		const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		void TransformFace(uint32_t faceID, const Mat4& delta);
		void UploadIfDirty();

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::vector<VertexPosColor> m_Vertices;
		std::vector<WORD> m_Indices;
		std::unordered_map<uint32_t, std::vector<uint32_t>> m_FaceToVertexIndices;

		bool m_IsDirty = false;
	};
}

