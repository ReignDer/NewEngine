#pragma once
#include "Vector3D.h"
#include "MathUtils.h"
#include <cmath>
namespace Sign {
	class Quaternion {
	public:
		Quaternion() : x(0), y(0), z(0), w(1) {}
		Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		
		static Quaternion QuaternionRotationPitchYawRoll(float pitch, float yaw, float roll) noexcept
		{
			const float halfpitch = pitch * 0.5f;
			float cp = cosf(halfpitch);
			float sp = sinf(halfpitch);

			const float halfyaw = yaw * 0.5f;
			float cy = cosf(halfyaw);
			float sy = sinf(halfyaw);

			const float halfroll = roll * 0.5f;
			float cr = cosf(halfroll);
			float sr = sinf(halfroll);

			return {
					cr * sp * cy + sr * cp * sy,
					cr * cp * sy - sr * sp * cy,
					sr * cp * cy - cr * sp * sy,
					cr * cp * cy + sr * sp * sy
				};
		}

		static Quaternion FromEulerAngles(const Vector3D& vec) noexcept {
			Vector3D halfAngles = { 0.5f * vec.x, 0.5f * vec.y, 0.5f * vec.z };

			const float sinX = std::sinf(halfAngles.x);
			const float cosX = std::cosf(halfAngles.x);
			const float sinY = std::sinf(halfAngles.y);
			const float cosY = std::cosf(halfAngles.y);
			const float sinZ = std::sinf(halfAngles.z);
			const float cosZ = std::cosf(halfAngles.z);


			return Quaternion( sinX * cosY * cosZ - cosX * sinY * sinZ,
				cosX * sinY * cosZ + sinX * cosY * sinZ,
				cosX * cosY * sinZ - sinX * sinY * cosZ,
				cosX * cosY * cosZ + sinX * sinY * sinZ );
		}
		static Quaternion FromEulerAnglesDegrees(const Vector3D& vec) noexcept {
			Vector3D vecRad = MathUtils::ConvertToRadiansVec3(vec);
			return FromEulerAngles(vecRad);
		}
		Quaternion operator*(const Quaternion& q) const noexcept
		{

			return {	(q.w * x) + (q.x * w) + (q.y * z) - (q.z * y),
						(q.w * y) - (q.x * z) + (q.y * w) + (q.z * x),
						(q.w * z) + (q.x * y) - (q.y * x) + (q.z * w),
						(q.w * w) - (q.x * x) - (q.y * y) - (q.z * z)
			};

		}

		Vector3D ToEulerAngles() const noexcept {
			float xx = x * x, yy = y * y, zz = z * z;
			float xy = x * y, xz = x * z, yz = y * z;
			float wx = w * x, wy = w * y, wz = w * z;

			float m00 = 1 - 2 * (yy + zz);
			float m01 = 2 * (xy + wz);
			float m02 = 2 * (xz - wy);

			float m10 = 2 * (xy - wz);
			float m11 = 1 - 2 * (xx + zz);
			float m12 = 2 * (yz + wx);

			float m22 = 1 - 2 * (xx + yy);


			float cos2 = m00 * m00 + m01 * m01;

			if (cos2 < 1e-6f) {
				return Vector3D(
					0.0f,
					m02 < 0 ? static_cast<float>(0.5f * MathUtils::PI) : static_cast<float>(-0.5f * MathUtils::PI),
					-std::atan2(m10, m11)
				);
			}
			else {
				return Vector3D(std::atan2(m12, m22),
					std::atan2(-m02, std::sqrt(cos2)),
					std::atan2(m01, m00));
			}
		}

		//https://fgiesen.wordpress.com/2019/02/09/rotating-a-single-vector-using-a-quaternion/
		Vector3D rotate(const Vector3D& vec) const noexcept
		{
			Vector3D p(x,y,z);
			Vector3D t = p.vectorProduct(vec) * 2.0f;
			return vec + (t*w) + p.vectorProduct(t);
		}
		
		Quaternion Conjugate() const { return { -x,-y,-z,w }; }

	public:
		float x{};
		float y{};
		float z{};
		float w{};
	};
}