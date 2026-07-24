#include "Primitive3D.h"

namespace Sign {
	namespace Primitive {
		std::shared_ptr<Mesh> Cube3D::Create(const std::array<Vector3D, 8>& color)
		{

			return ResourceCache::GetOrCreate<Mesh>("DefaultCube", [&]()->std::shared_ptr<Mesh> {
				VertexPosColor CubeVertices[8];

				for (size_t i = 0; i < 8; i++) {
					CubeVertices[i] = { cubePosition[i], Vector3D(0,0,0), color[i]};
				}

				return std::make_shared<Mesh>(CubeVertices, _countof(CubeVertices), cubeIndices, _countof(cubeIndices));
				});

		}
		std::shared_ptr<Mesh> Sphere::Create(const Vector3D& Color)
		{
			return ResourceCache::GetOrCreate<Mesh>("DefaultSphere", [&]()->std::shared_ptr<Mesh> {
				//Iconosphere
				const float H_ANGLE = MathUtils::ConvertToRadians(72.0f);
				const float V_ANGLE = std::atanf(1.0f / 2.0f);
				float radius = 1.0f;

				std::vector<float> vertices;
				std::vector<uint32_t> indices;
				int row_1, row_2;
				float z, xy;
				float hAngle1 = -MathUtils::PI / 2 - H_ANGLE / 2;
				float hAngle2 = -MathUtils::PI / 2;

				vertices.insert(vertices.end(), { 0.0f,0.0f,radius });

				for (int i = 0; i < 5; i++) {

					z = radius * std::sin(V_ANGLE);
					xy = radius * std::cos(V_ANGLE);

					vertices.push_back(xy * std::cos(hAngle1));
					vertices.push_back(xy * std::sin(hAngle1));
					vertices.push_back(z);


					hAngle1 += H_ANGLE;


				}
				for (int i = 0; i < 5; i++) {

					z = radius * std::sin(V_ANGLE);
					xy = radius * std::cos(V_ANGLE);
					vertices.push_back(xy * std::cos(hAngle2));
					vertices.push_back(xy * std::sin(hAngle2));
					vertices.push_back(-z);

					hAngle2 += H_ANGLE;

				}

				vertices.insert(vertices.end(), { 0.0f,0.0f,-radius });

				for (int i = 1; i <= 5; i++) {
					int next = i % 5 + 1;
					int bot = i + 5;
					int botNext = next + 5;
					AddIndices(indices, 0, i, next); // Top
					AddIndices(indices, i, bot, next); // Upper Middle
					AddIndices(indices, bot, botNext, next); // Lower Middle
					AddIndices(indices, 11, botNext, bot); // Bot
				}

				const int SUBDIVISION = 3;
				for (int s = 0; s < SUBDIVISION; s++) {
					std::vector<float> tmpVertices = vertices;
					std::vector<uint32_t> tmpIndices = indices;
					vertices.clear();
					indices.clear();
					uint32_t index = 0;
					for (int i = 0; i < (int)tmpIndices.size(); i += 3)
					{
						const float* v1 = &tmpVertices[tmpIndices[i] * 3];
						const float* v2 = &tmpVertices[tmpIndices[i + 1] * 3];
						const float* v3 = &tmpVertices[tmpIndices[i + 2] * 3];

						std::vector<float> newVs;
						newVs.insert(newVs.end(), v1, v1 + 3);

						for (int j = 1; j <= SUBDIVISION; j++)
						{
							float a = (float)j / SUBDIVISION;
							float newV1[3], newV2[3], newV3[3];
							Interpolate(v1, v2, a, radius, newV1);
							Interpolate(v1, v3, a, radius, newV2);

							newVs.insert(newVs.end(), newV1, newV1 + 3);
							for (int k = 1; k < j; k++)
							{
								float b = (float)k / j;
								Interpolate(newV1, newV2, b, radius, newV3);
								newVs.insert(newVs.end(), newV3, newV3 + 3);
							}
							newVs.insert(newVs.end(), newV2, newV2 + 3);
						}

						for (int j = 1; j <= SUBDIVISION; j++)
						{
							for (int k = 0; k < j; k++)
							{
								uint32_t i1 = (j - 1) * j / 2 + k;
								uint32_t i2 = j * (j + 1) / 2 + k;

								const float* sv1 = &newVs[i1 * 3];
								const float* sv2 = &newVs[i2 * 3];
								const float* sv3 = &newVs[(i2 + 1) * 3];
								vertices.insert(vertices.end(), sv1, sv1 + 3);
								vertices.insert(vertices.end(), sv2, sv2 + 3);
								vertices.insert(vertices.end(), sv3, sv3 + 3);
								AddIndices(indices, index, index + 1, index + 2);
								index += 3;

								if (k < j - 1)
								{
									uint32_t i2b = i1 + 1;
									const float* sv2b = &newVs[i2b * 3];
									vertices.insert(vertices.end(), sv1, sv1 + 3);
									vertices.insert(vertices.end(), sv3, sv3 + 3);
									vertices.insert(vertices.end(), sv2b, sv2b + 3);
									AddIndices(indices, index, index + 1, index + 2);
									index += 3;
								}
							}
						}
					}

				}
				std::vector<VertexPosColor> finalVertices;
				for (int i = 0; i < (int)vertices.size(); i += 3) {
					VertexPosColor v;
					v.Position = { vertices[i],vertices[i + 2], vertices[i + 1] };
					v.Color = Color;
					finalVertices.push_back(v);
				}

				return std::make_shared<Mesh>(finalVertices.data(), (uint32_t)finalVertices.size(), indices.data(), (uint32_t)indices.size());
				});
		}
		void Sphere::AddIndices(std::vector<uint32_t>& indices, int i1, int i2, int i3)
		{
			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);
		}
		void Sphere::Interpolate(const float* v1, const float* v2, float t, float radius, float* out)
		{
			out[0] = v1[0] + (v2[0] - v1[0]) * t;
			out[1] = v1[1] + (v2[1] - v1[1]) * t;
			out[2] = v1[2] + (v2[2] - v1[2]) * t;

			float len = std::sqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2]);
			out[0] = out[0] / len * radius;
			out[1] = out[1] / len * radius;
			out[2] = out[2] / len * radius;
		}
		std::shared_ptr<Mesh> Plane::Create(const std::array<Vector3D, 4>& color)
		{
			return ResourceCache::GetOrCreate<Mesh>("DefaultPlane", [&]()->std::shared_ptr<Mesh> {
				VertexPosColor planeVertices[4];

				for (size_t i = 0; i < 4; i++) {
					planeVertices[i] = { planePosition[i], Vector3D(0,0,0), color[i]};
				}
				return std::make_shared<Mesh>(planeVertices, _countof(planeVertices), quadIndices, _countof(quadIndices));
				});
		}
	}
}