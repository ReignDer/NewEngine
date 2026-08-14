#pragma once
#include <array>
#include <memory>
#include <vector>
#include "Sign/Renderer/Mesh.h"
#include "Sign/Renderer/MeshUtils.h"
#include "Sign/Math/SignMath.h"
#include "Sign/Renderer/ResourceCache.h"
namespace Sign {
	enum class PrimitiveTypes
	{
		None = 0,
		Cube,
		Sphere,
		Plane,
		Capsule
	};
	namespace Primitive {
		static const std::array<Vector3D, 8> cubePosition = { {
			{-0.5,-0.5,-0.5} , {-0.5,0.5,-0.5 },
			{ 0.5,0.5,-0.5 } , { 0.5,-0.5,-0.5 },
			{ -0.5,-0.5,0.5 }, { -0.5,0.5,0.5 },
			{ 0.5,0.5,0.5 }  , { 0.5,-0.5,0.5 }
		} };
		static const std::array<Vector3D, 4> planePosition = { {
			{-5.0,0.0,5.0 } ,
			{ 5.0,0.0,5.0 } ,
			{ 5.0,0.0,-5.0 },
			{ -5.0,0.0,-5.0 }
		} };
		struct Cube3D
		{
		public:
			static std::shared_ptr<Mesh> Create(
				const std::array<Vector3D,8>& color = {{
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f}}}
				);

		};

		struct Sphere
		{
		public:
			static std::shared_ptr<Mesh> Create(
				const Vector3D& centerColor = Vector3D(1.0f, 1.0f, 1.0f));

		private:
			static void AddIndices(std::vector<uint32_t>& indices, int i1, int i2, int i3);
			static void Interpolate(const float* v1, const float* v2, float t, float radius, float* out);
		};

		struct Plane {
		public:
			static std::shared_ptr<Mesh> Create(
				const std::array<Vector3D, 4>& color = { {
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f}} }
					);
		};

		struct Capsule
		{
			static std::shared_ptr<Mesh> Create();

		private:
			static void CalculateRings(size_t segments, float r, float y, float dy, float radius, float height, size_t faceID,
				 std::vector<Vector3D>& positions,  std::vector<Vector3D>& normals,  std::vector<Vector2D>& uvs,  std::vector<Vector3D>& colors, std::vector<VertexPosColor>& vertices);
		};
	};
}

