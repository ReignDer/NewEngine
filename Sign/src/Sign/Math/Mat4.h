#pragma once
#include <cmath>
#include "Vector3D.h"

namespace Sign {
	class Mat4
	{
	public:
		Mat4() = default;

		static Mat4 identity() noexcept
		{
			Mat4 res{};
			res.matrix[0][0] = 1;
			res.matrix[1][1] = 1;
			res.matrix[2][2] = 1;
			res.matrix[3][3] = 1;
			return res;
		}
		static Mat4 translate(const Vector3D& vec) noexcept
		{
			Mat4 res = Mat4::identity();
			res.matrix[3][0] = vec.x;
			res.matrix[3][1] = vec.y;
			res.matrix[3][2] = vec.z;
			return res;
		}
		static Mat4 scale(const Vector3D& vec) noexcept
		{
			Mat4 res{};
			res.matrix[0][0] = vec.x;
			res.matrix[1][1] = vec.y;
			res.matrix[2][2] = vec.z;
			res.matrix[3][3] = 1;
			return res;
		}
		static Mat4 fromQuaternion(const Quaternion& q) noexcept
		{
			Mat4 res{};
			float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
			float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
			float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

			res.matrix[0][0] = 1 - 2 * (yy+zz);
			res.matrix[0][1] = 2 * (xy + wz);
			res.matrix[0][2] = 2 * (xz - wy);

			res.matrix[1][0] = 2 * (xy - wz);
			res.matrix[1][1] = 1 - 2 * (xx + zz);
			res.matrix[1][2] = 2 * (yz + wx);

			res.matrix[2][0] = 2 * (xz + wy);
			res.matrix[2][1] = 2 * (yz - wx);
			res.matrix[2][2] = 1 - 2 * (xx + yy);

			res.matrix[3][3] = 1;

			return res;
		}


		static Mat4 rotateX(float x) noexcept 
		{
			Mat4 res{};
			float cos = std::cos(x);
			float sin = std::sin(x);
			res.matrix[0][0] = 1;
			res.matrix[1][1] = cos;
			res.matrix[1][2] = sin;
			res.matrix[2][1] = -sin;
			res.matrix[2][2] = cos;
			res.matrix[3][3] = 1;
			return res;
		}
		static Mat4 rotateY(float y) noexcept 
		{
			Mat4 res{};
			float cos = std::cos(y);
			float sin = std::sin(y);
			res.matrix[0][0] = cos;
			res.matrix[2][0] = sin;
			res.matrix[1][1] = 1;
			res.matrix[0][2] = -sin;
			res.matrix[2][2] = cos;
			res.matrix[3][3] = 1;
			return res;
		}
		static Mat4 rotateZ(float z) noexcept 
		{
			Mat4 res{};
			float cos = std::cos(z);
			float sin = std::sin(z);
			res.matrix[0][0] = cos;
			res.matrix[0][1] = sin;
			res.matrix[1][0] = -sin;
			res.matrix[1][1] = cos;
			res.matrix[2][2] = 1;
			res.matrix[3][3] = 1;
			return res;
		}
		static Mat4 orthoLH(float width, float height, float zNear, float zFar) noexcept
		{
			Mat4 res{};
			res.matrix[0][0] = 2.0f / width;
			res.matrix[1][1] = 2.0f / height;
			res.matrix[2][2] = 1.0f / (zFar - zNear);
			res.matrix[3][2] = -(zNear / (zFar - zNear));
			res.matrix[3][3] = 1;
			return res;
		}

		static Mat4 perspectiveFovLH(float fov, float aspect, float zNear, float zFar)
		{
			Mat4 res{};
			float yScale = 1.0f / std::tan(fov / 2.0f);
			float xScale = yScale / aspect;
			res.matrix[0][0] = xScale;
			res.matrix[1][1] = yScale;
			res.matrix[2][2] = zFar / (zFar - zNear);
			res.matrix[2][3] = 1.0f;
			res.matrix[3][2] = (-zNear * zFar) / (zFar - zNear);
			res.matrix[3][3] = 0.0f;
			return res;

		}

		static Mat4 lookAtLH(const Vector3D& eye, const Vector3D& target, const Vector3D& up) noexcept 
		{
			auto zAxis = (target - eye).normalize();
			auto xAxis = up.vectorProduct(zAxis).normalize();
			auto yAxis = zAxis.vectorProduct(xAxis);

			Mat4 res{};
			res.matrix[0][0] = xAxis.x;
			res.matrix[1][0] = xAxis.y;
			res.matrix[2][0] = xAxis.z;

			res.matrix[0][1] = yAxis.x;
			res.matrix[1][1] = yAxis.y;
			res.matrix[2][1] = yAxis.z;

			res.matrix[0][2] = zAxis.x;
			res.matrix[1][2] = zAxis.y;
			res.matrix[2][2] = zAxis.z;

			res.matrix[3][0] = -xAxis.dotProduct(eye);
			res.matrix[3][1] = -yAxis.dotProduct(eye);
			res.matrix[3][2] = -zAxis.dotProduct(eye);
			res.matrix[3][3] = 1.0f;

			return res;
		}
		static Mat4 transpose(const Mat4& m) noexcept 
		{
			Mat4 res{};
			for (auto row = 0u; row < 4u; row++) {
				for (auto col = 0u; col < 4u; col++) {
					res.matrix[row][col] = m.matrix[col][row];
				}
			}

			return res;
		}
		Mat4 operator*(const Mat4& rhs) const noexcept
		{
			Mat4 res{};
			for (auto row = 0u; row < 4u; row++)
			{
				for (auto k = 0u; k < 4u; k++) {
					auto temp = matrix[row][k];
					for (auto col = 0u; col < 4u; col++) {
						res.matrix[row][col] += temp * rhs.matrix[k][col];
					}
				}
			}
			return res;
		}

		Vector3D TransformPoint(const Vector3D& v) const noexcept
		{
			// row-vector convention: result = v * M  (w assumed = 1)
			float x = v.x * matrix[0][0] + v.y * matrix[1][0] + v.z * matrix[2][0] + matrix[3][0];
			float y = v.x * matrix[0][1] + v.y * matrix[1][1] + v.z * matrix[2][1] + matrix[3][1];
			float z = v.x * matrix[0][2] + v.y * matrix[1][2] + v.z * matrix[2][2] + matrix[3][2];
			return Vector3D(x, y, z);
		}

	private:
		float matrix[4][4]{};
	};
}

