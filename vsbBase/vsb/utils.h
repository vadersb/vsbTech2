//
// Created by Alexander on 29.09.2025.
//

#pragma once
#include <type_traits>

namespace vsb
{
	template<typename TTo, typename TFrom>
	TTo* SafeCast(TFrom* sourcePointer)
	{
		if constexpr (std::is_same_v<TFrom, TTo>)
		{
			return sourcePointer;
		}

		if (sourcePointer == nullptr)
		{
			return nullptr;
		}

		if constexpr (std::is_convertible_v<TFrom*, TTo*>)
		{
			return static_cast<TTo*>(sourcePointer);
		}
		else if constexpr (std::is_polymorphic_v<TFrom> && std::is_polymorphic_v<TTo>)
		{
			return dynamic_cast<TTo*>(sourcePointer);
		}
		else
		{
			static_assert(false, "no safe cast way is found for types");
		}

		return nullptr;
	}

}
