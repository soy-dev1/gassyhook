#pragma once
#include <numbers>
#include <cstdint>
#include <cmath>

template <typename T> constexpr auto deg2rad(T degrees) noexcept { return degrees * (std::numbers::pi_v<T> / static_cast<T>(180)); }
template <typename T> constexpr auto rad2deg(T radians) noexcept { return radians * (static_cast<T>(180) / std::numbers::pi_v<T>); }

class CMatrix3x4;

class CVector
{

public:
	constexpr CVector operator+(const CVector& other) const noexcept
	{
		return { x + other.x, y + other.y, z + other.z };
	}

	constexpr CVector operator+=(const CVector& other) const noexcept
	{
		return { x + other.x, y + other.y, z + other.z };
	}

	constexpr CVector operator-(const CVector& other) const noexcept
	{
		return { x - other.x, y - other.y, z - other.z };
	}

	constexpr CVector operator*(const CVector& other) const noexcept
	{
		return { x * other.x, y * other.y, z * other.z };
	}

	constexpr CVector operator/(const CVector& other) const noexcept
	{
		return { x / other.x, y / other.y, z / other.z };
	}

	// Access by index
	float& operator[](unsigned index) {
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;

		// Throw exception or other error handling
	}

	// Const access  
	const float& operator[](unsigned index) const {
		if (index == 0) return x;
		if (index == 1) return y;
		if (index == 2) return z;

		// Throw exception or other error handling
	}

	constexpr CVector Scale(float factor) const noexcept
	{
		return { x * factor, y * factor, z * factor };
	}

	auto length2D() const noexcept
	{
		return std::sqrt(x * x + y * y);
	}

	static auto fromAngle(const CVector& angle) noexcept
	{
		return CVector{ std::cos(deg2rad(angle.x)) * std::cos(deg2rad(angle.y)),
						std::cos(deg2rad(angle.x)) * std::sin(deg2rad(angle.y)),
						-std::sin(deg2rad(angle.x)) };
	}

	inline CVector ToAngle() const noexcept
	{
		return {
			std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
			std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>),
			0.0f 
		};
	}

	CVector& normalize() noexcept
	{
		x = std::isfinite(x) ? std::remainder(x, 360.0f) : 0.0f;
		y = std::isfinite(y) ? std::remainder(y, 360.0f) : 0.0f;
		z = 0.0f;
		return *this;
	}

	constexpr auto dotProduct(const CVector& v) const noexcept
	{
		return x * v.x + y * v.y + z * v.z;
	}

	float x{ }, y{ }, z{ };
};


class VectorAligned : public CVector
{
public:
	VectorAligned()
	{
		x = y = z = 0.0f;
	}

	VectorAligned(const CVector& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	float w;
};


__declspec(align(16)) class CVectorAligned : public CVector
{
public:
	constexpr CVectorAligned operator-(const CVectorAligned& other) const noexcept
	{
		return { x - other.x, y - other.y, z - other.z, w - other.w };
	}

	float w{ };
};
