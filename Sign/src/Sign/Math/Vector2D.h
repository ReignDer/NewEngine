#pragma once

namespace Sign {
	class Vector2D {
	public:
		Vector2D() : x(0), y(0) {}
		Vector2D(float x, float y) : x(x), y(y) {}
		void operator+= (const Vector2D& v) noexcept
		{
			this->x += v.x;
			this->y += v.y;
		}

		void operator-= (const Vector2D& v)noexcept
		{
			this->x -= v.x;
			this->y -= v.y;
		}

		//Addition
		Vector2D operator+ (const Vector2D& v) const noexcept { return Vector2D(this->x + v.x, this->y + v.y); }

		//Subtraction
		Vector2D operator- (const Vector2D& v) const noexcept { return Vector2D(this->x - v.x, this->y - v.y); }

		//Divide
		Vector2D operator/ (const float v) const noexcept { return Vector2D(this->x / v, this->y / v); }
		
		Vector2D operator* (const float scalarMul) const noexcept { return Vector2D(this->x * scalarMul, this->y * scalarMul); }

	public:
		float x{};
		float y{};
	};
}
