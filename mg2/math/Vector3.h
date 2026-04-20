#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>

class Vector3 {
public:
	float x, y, z;
	Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {};

	Vector3 operator+(const Vector3& v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator+(float scalar) const {
		return Vector3(x + scalar, y + scalar, z + scalar);
	}

	Vector3 operator*(float scalar) const {
		return Vector3(scalar * x, scalar * y, scalar * z);
	}

	Vector3 operator-(const Vector3& v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	float operator*(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	Vector3 operator-() const { 
		return { -x, -y, -z };
	}

	Vector3 operator/(const Vector3&v) const {
		if (std::abs(v.x) <= FLT_EPSILON ||
			std::abs(v.y) <= FLT_EPSILON ||
			std::abs(v.z) <= FLT_EPSILON) {
			throw std::exception("Can't divide by 0!");
		}

		return { x / v.x, y / v.y, z / v.z };
	}

	bool operator==(const Vector3& v) const {
		return std::abs(v.x - x) < FLT_EPSILON &&
			std::abs(v.y - y) < FLT_EPSILON &&
			std::abs(v.z - z) < FLT_EPSILON;
	}

	Vector3 operator/(float scalar) const {
		if (scalar <= FLT_EPSILON) {
			throw std::exception("Can't divide by 0!");
		}
		return { x / scalar, y / scalar, z / scalar };
	}

	Vector3 cross(const Vector3& v) const {
		return Vector3(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}
	
	static Vector3 lerp(const Vector3& a, const Vector3& b, float t) {
		t = std::clamp(t, 0.0f, 1.0f);
		return a + (b - a) * t;
	}

	float dot(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	float length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	Vector3 normalize() const {
		float len = length();

		if (std::abs(len) <= FLT_EPSILON) throw std::exception("Length of vector can't be 0");
		
		return (*this) * (1 / len);
	}

	void print() const {
		std::cout << "(" << x << ", " << y << ", " << z << ")\n";
	}
};