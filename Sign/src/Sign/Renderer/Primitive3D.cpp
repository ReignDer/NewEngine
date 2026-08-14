#include "Primitive3D.h"

namespace Sign {
	namespace Primitive {
		std::shared_ptr<Mesh> Cube3D::Create(const std::array<Vector3D, 8>& color)
		{

			return ResourceCache::GetOrCreate<Mesh>("DefaultCube", [&]()->std::shared_ptr<Mesh> {
				VertexPosColor CubeVertices[24];
				std::array<Vector3D,24> cubePositions = {
					//Back Face
					cubePosition[0], cubePosition[1],cubePosition[2], cubePosition[3],
					//Front Face
					cubePosition[4], cubePosition[5], cubePosition[6], cubePosition[7],
					//Left Face
					cubePosition[0], cubePosition[1], cubePosition[5], cubePosition[4],
					//Right Face
					cubePosition[3], cubePosition[2], cubePosition[6], cubePosition[7],
					//Top Face
					cubePosition[1], cubePosition[5], cubePosition[6], cubePosition[2],
					//Bottom Face
					cubePosition[0], cubePosition[4], cubePosition[7], cubePosition[3]
				};
				std::array<Vector2D, 4> uv = {
					Vector2D(0,0),
					Vector2D(1,0),
					Vector2D(1,1),
					Vector2D(0,1)
				};
				std::array<Vector3D, 6> faceNormals = {
					Vector3D(0,0,-1),
					Vector3D(1,0,1),
					Vector3D(-1,0,0),
					Vector3D(1,0,0),
					Vector3D(0,1,0),
					Vector3D(0,-1,0)
				};
				for (size_t i = 0; i < 24; i++) {

					size_t uvIndex = i % 4;
					size_t faceIndex = i / 4;
					CubeVertices[i] = { cubePositions[i], faceNormals[faceIndex], Vector3D(0.34,0.65,1), uv[uvIndex], (unsigned int)i};
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
				float radius = 0.5f;

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
					Vector3D normals = Vector3D(vertices[i], vertices[i + 2], vertices[i + 1]).normalize();
					v.Normals = normals;
					v.TexCoord = Vector2D(0, 0);
					v.FaceID = i;
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
				std::array<Vector2D, 4> uv = {
					Vector2D(0,0),
					Vector2D(1,0),
					Vector2D(1,1),
					Vector2D(0,1)
				};
				for (size_t i = 0; i < 4; i++) {
					planeVertices[i] = { planePosition[i], Vector3D(0,1,0), color[i], uv[i], (unsigned int)i};
				}
				return std::make_shared<Mesh>(planeVertices, _countof(planeVertices), quadIndices, _countof(quadIndices));
				});
		}
		std::shared_ptr<Mesh> Capsule::Create()
		{

			return ResourceCache::GetOrCreate<Mesh>("DefaultCapsule", [&]()->std::shared_ptr<Mesh> {
				float radius = 0.5f;
				float height = radius * 2.f;
				int numSubdivisionsHeight = 12;
				int numSegments = 12;

				std::vector<VertexPosColor> vertices;
				std::vector<Vector3D> positions;
				std::vector<Vector3D> normals;
				std::vector<Vector2D> uvs;
				std::vector<Vector3D> colors;
				std::vector<uint32_t> indices;

				auto ringBody = numSubdivisionsHeight + 1;
				auto ringTotal = numSubdivisionsHeight + ringBody;

				positions.reserve(numSegments * ringTotal);
				normals.reserve(numSegments * ringTotal);
				uvs.reserve(numSegments * ringTotal);
				colors.reserve(numSegments * ringTotal);

				indices.reserve((numSegments - 1) * (ringTotal - 1) * 6);

				auto bodyIncr = 1.0f / (float)(ringBody - 1);
				auto ringIncr = 1.0f / (float)(numSubdivisionsHeight - 1);

				int currentRingIndex = 0;
				for (int r = 0; r < numSubdivisionsHeight / 2; r++) {
					CalculateRings(numSegments, std::sin(MathUtils::PI * r * ringIncr), std::sin(MathUtils::PI * (r * ringIncr - 0.5f)), -0.5f, radius, height, currentRingIndex,
						positions, normals, uvs, colors, vertices);

					currentRingIndex++;
				}
				for (size_t r = 0; r < ringBody; r++) {
					CalculateRings(numSegments, 1.0f, 0.0f, r * bodyIncr - 0.5f, radius, height, currentRingIndex, positions, normals, uvs, colors, vertices);
					currentRingIndex++;
				}
				for (int r = numSubdivisionsHeight / 2; r < numSubdivisionsHeight; r++) {
					CalculateRings(numSegments, std::sin(MathUtils::PI * r * ringIncr), std::sin(MathUtils::PI * (r * ringIncr - 0.5f)), +0.5f, radius, height, currentRingIndex,
						positions, normals, uvs, colors, vertices);
					currentRingIndex++;
				}

				for (size_t r = 0; r < ringTotal - 1; r++) {
					for (int s = 0; s < numSegments - 1; s++) {
						indices.push_back((uint32_t)(r * numSegments + (s + 1)));
						indices.push_back((uint32_t)(r * numSegments + (s + 0)));
						indices.push_back((uint32_t)((r + 1) * numSegments + (s + 1)));

						indices.push_back((uint32_t)((r + 1) * numSegments + (s + 0)));
						indices.push_back((uint32_t)((r + 1) * numSegments + (s + 1)));
						indices.push_back((uint32_t)(r * numSegments + s));
					}
				}


				return std::make_shared<Mesh>(vertices.data(), (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size());
			});
		}
		void Capsule::CalculateRings(size_t segments, float r, float y, float dy, float radius, float height, size_t faceID,
			 std::vector<Vector3D>& positions,  std::vector<Vector3D>& normals,  std::vector<Vector2D>& uvs,  std::vector<Vector3D>& colors, std::vector<VertexPosColor>& vertices)
		{
			float segIncr = 1.0f / (float)(segments - 1);

			for (size_t s = 0; s < segments; s++)
			{
				float x = std::cosf((MathUtils::PI * 2) * s * segIncr) * r;
				float z = std::sinf((MathUtils::PI * 2) * s * segIncr) * r;

				positions.emplace_back(Vector3D(radius * x, radius * y + height * dy, radius * z));
				normals.emplace_back(Vector3D(x, y, z));
				float u = 1.0f - (s * segIncr);
				float v = 0.5f - (radius * y + height * dy) / (2.0f * radius + height);
				uvs.emplace_back(Vector2D(u, v));
				//float g = 0.5f + (radius * y + height * dy) / (2.0f * radius + height);
				colors.emplace_back(1.f, 1.f, 1.f);
				vertices.emplace_back(VertexPosColor{ positions.back(), normals.back(), colors.back(), uvs.back(), (uint32_t)faceID});
			}
		}
	}
}