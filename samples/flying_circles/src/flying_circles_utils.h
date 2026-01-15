//
// Created by Alexander on 12.01.2026.
//

#pragma once

#include <compare>

namespace fc_utils
{
	float GetRandomFloat(float min, float max);
	float GetRandomFloat01();

	template<typename T>
	struct PointerWithIndex
	{
		T* pPointer;
		int index;
	};

}