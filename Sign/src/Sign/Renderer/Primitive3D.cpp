#include "Primitive3D.h"

namespace Sign {
	namespace Primitive {
		std::shared_ptr<Mesh> Cube3D::Create(const DirectX::XMFLOAT3& translation, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& rotation, const std::array<DirectX::XMFLOAT3, 8>& color)
		{
			VertexPosColor CubeVertices[8];

			for (size_t i = 0; i < 8; i++) {
				CubeVertices[i] = {cubePosition[i], color[i]};
			}


			return std::make_shared<Mesh>(CubeVertices,_countof(CubeVertices),cubeIndices,_countof(cubeIndices),translation,scale,rotation);
		}

		std::shared_ptr<Mesh> Sphere::Create(const DirectX::XMFLOAT3& translation, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& rotation, const std::array<DirectX::XMFLOAT3, 8>& color)
		{
			return std::shared_ptr<Mesh>();
		}
		std::shared_ptr<Mesh> Plane::Create(const DirectX::XMFLOAT3& translation, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& rotation, const std::array<DirectX::XMFLOAT3, 4>& color)
		{
			VertexPosColor planeVertices[4];

			for (size_t i = 0; i < 4; i++) {
				planeVertices[i] = { planePosition[i], color[i] };
			}
			return std::make_shared<Mesh>(planeVertices, _countof(planeVertices), quadIndices, _countof(quadIndices), translation, scale, rotation);
		}
	}
}