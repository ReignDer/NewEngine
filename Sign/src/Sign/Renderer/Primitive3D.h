#pragma once
#include <array>
#include <memory>
#include <vector>
#include "Sign/Renderer/Mesh.h"
#include "Sign/Renderer/MeshUtils.h"
#include "Sign/Math/SignMath.h"
#include "Sign/Renderer/ResourceCache.h"
namespace Sign {
	namespace Primitive {
		static const std::array<Vector3D, 8> cubePosition = { {
			{-0.5,-0.5,-0.5} , {-0.5,0.5,-0.5 },
			{ 0.5,0.5,-0.5 } , { 0.5,-0.5,-0.5 },
			{ -0.5,-0.5,0.5 }, { -0.5,0.5,0.5 },
			{ 0.5,0.5,0.5 }  , { 0.5,-0.5,0.5 }
		} };
		static const std::array<Vector3D, 4> planePosition = { {
			{-0.5,0.0,0.5 } ,
			{ 0.5,0.0,0.5 } ,
			{ 0.5,0.0,-0.5 },
			{ -0.5,0.0,-0.5 }
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
	};
}

