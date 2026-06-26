#pragma once
#include <numbers>
#include <random>
#include "Sign/Math/Vector3D.h"
namespace MathUtils {
	inline auto PI = std::numbers::pi_v<float>;

	inline float ConvertToRadians(float degrees)
	{
		return degrees * (PI / 180.0f);
	}

	inline float Random_Float(float min, float max) {
		thread_local std::random_device rd;
		thread_local std::mt19937 gen(rd());

		std::uniform_real_distribution<float> distrib(min, max);
		return distrib(gen);
	}

	inline int Random_Int(int min, int max) {
		thread_local std::random_device rd;
		thread_local std::mt19937 gen(rd());

		std::uniform_int_distribution<int> distrib(min, max);
		return distrib(gen);
	}

	inline Sign::Vector3D lerp(const Sign::Vector3D& a, const Sign::Vector3D& b, float t) {
		return { a.x + (b.x - a.x) * t,
				a.y + (b.y - a.y) * t,
				a.z + (b.z - a.z) * t};
	}
}
