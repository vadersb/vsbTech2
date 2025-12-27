//
// Created by Alexander on 22.12.2025.
//

#pragma once

#include <vector>

namespace vsb
{
	template<typename T>
	std::vector<T> ReserveEmptyVector(size_t size)
	{
		std::vector<T> result(size);
		result.reserve(size);
		return result;
	}
}