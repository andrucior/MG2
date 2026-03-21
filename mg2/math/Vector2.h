#pragma once
#include "cmath"
#include <iostream>

class Vector2 {
public:
	float x, y;
	Vector2(float x = 0, float y = 0) : x(x), y(y) {};

	Vector2 operator+(const Vector2& v) const {
		return Vector2(x + v.x, y + v.y);
	}

	Vector2 operator+(float scalar) const {
		return Vector2(x + scalar, y + scalar);
	}

	Vector2 operator*(float scalar) const {
		return Vector2(scalar * x, scalar * y);
	}

	Vector2 operator-(const Vector2& v) const {
		return Vector2(x - v.x, y - v.y);
	}

	float operator*(const Vector2& v) const {
		return x * v.x + y * v.y;
	}

	Vector2 operator-() const {
		return { -x, -y };
	}

	Vector2 cross(const Vector2& v) const {
		return Vector2(
			y * v.x - x * v.y,
			x * v.y - y * v.x
		);
	}

	float dot(const Vector2& v) const {
		return x * v.x + y * v.y;
	}

	float length() const {
		return std::sqrt(x * x + y * y);
	}

	Vector2 normalize() const {
		float len = length();

		if (len == 0) return Vector2();

		return (*this) * (1 / len);
	}

	void print() const {
		std::cout << "(" << x << ", " << y << ", " << ")\n";
	}
};