#include "Primitive2D.h"

namespace Sign {
	namespace Primitive {
		std::shared_ptr<Mesh> Circle::Create(const Vector3D& centerColor, const Vector3D& outerColor)
		{
			std::vector<VertexPosColor> vertices;
			std::vector<uint32_t> indices;

			VertexPosColor centerVertex;
			centerVertex.Position = Vector3D(0.0f, 0.0f, 0.0f);
			centerVertex.Color = centerColor;

			vertices.push_back(centerVertex);

			for (int i = 0; i < 64; i++) {
				float angle = ((float)i / (float)64 * 2.0f * MathUtils::PI);

				VertexPosColor perimeterVertex;
				perimeterVertex.Position.x = std::cos(angle) * 1.0f;
				perimeterVertex.Position.y = std::sin(angle) * 1.0f;
				perimeterVertex.Position.z = 0.0f;
				perimeterVertex.Color = outerColor;

				vertices.push_back(perimeterVertex);
			}

			for (int i = 1; i < 64; i++) {
				indices.push_back(0);
				indices.push_back(i);
				indices.push_back(i + 1);
			}

			indices.push_back(0);
			indices.push_back(64);
			indices.push_back(1);

			std::println("VCount: {} , ICount: {}", vertices.size(), indices.size());
			return std::make_shared<Mesh>(vertices.data(), (uint32_t)vertices.size(),indices.data(), (uint32_t)indices.size());
		}
		std::shared_ptr<Mesh> Triangle::Create(const std::array<Vector3D, 3>& color)
		{
			VertexPosColor trianlgeVertices[3];

			for (size_t i = 0; i < 3; i++) {
				trianlgeVertices[i] = { trianglePosition[i], color[i] };
			}

			return std::make_shared<Mesh>(trianlgeVertices, _countof(trianlgeVertices), trianlgeIndices, _countof(trianlgeIndices));
		}
	}
}


