//
// Created by Alexander on 12.01.2026.
//

#pragma once

#include <compare>

namespace fc_utils
{
	float GetRandomFloat(float min, float max);
	float GetRandomFloat01();

	void MyRaylibLogCallback(int logLevel, const char* text, va_list args);

	template<typename T>
	struct PointerWithIndex
	{
		T* pPointer;
		int index;
	};

}