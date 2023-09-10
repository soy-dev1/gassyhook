#pragma once

#include "cvector.h"

class CMatrix3x4
{
public:
	constexpr float* operator[](size_t index) noexcept
	{
		return data[index];
	}

	constexpr float* operator[](size_t index) const noexcept
	{
		return const_cast<float*>(data[index]);
	}


	constexpr CVector Origin() noexcept 
	{
		return { data[0][3], data[1][3], data[2][3] };
	}

	__forceinline float* Base() {
		return &data[0][0];
	}

	__forceinline const float* Base() const {
		return &data[0][0];
	}

	float data[3][4];
};

class CMatrix4x4
{
public:
	float data[4][4];
};
