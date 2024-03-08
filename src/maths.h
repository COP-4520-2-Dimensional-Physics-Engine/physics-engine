#pragma once
#include <cmath>

struct vec2 {
	double x, y;

	vec2(double s = 0) : x(s), y(s) {}
	vec2(double x, double y) : x(x), y(y) {}

	vec2 operator+(const vec2 &other) const { return vec2(x + other.x, y + other.y); }
	vec2 operator-(const vec2 &other) const { return vec2(x - other.x, y - other.y); }
	vec2 operator*(const vec2 &other) const { return vec2(x * other.x, y * other.y); }
	vec2 operator/(const vec2 &other) const { return vec2(x / other.x, y / other.y); }

	double dot(const vec2 &other) const {
		return x * other.x + y * other.y;
	}

	double cross(const vec2 &other) const {
		return x * other.y - y * other.x;
	}

	double length() const {
		return std::sqrt(dot(*this));
	}

	vec2 unit() const {
		return *this / vec2(length());
	}

	bool operator==(const vec2 &other) const {
		return x == other.x && y == other.y;
	}

	bool operator!=(const vec2 &other) const {
		return !(*this == other);
	}

};
