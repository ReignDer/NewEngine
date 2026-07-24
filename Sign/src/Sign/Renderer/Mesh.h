#pragma once
#include <cstdint>
#include "Sign/Buffers/VertexArray.h"
#include "Sign/Math/SignMath.h"
#include "Sign/Asset/Asset.h"

namespace Sign {
	class Mesh : public Asset
	{
	public:
		Mesh(const void* vertices, uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount);
		~Mesh() = default;
		const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		static AssetType GetStaticType() { return AssetType::Mesh; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	private:
		std::shared_ptr<VertexArray> m_VertexArray;

	};
}

