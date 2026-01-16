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

		//is here just to stop the compiler from complaining
		return nullptr;
	}


	template<int MaxN, typename F>
	constexpr void TemplateIndexedDispatch(int n, F&& func)
	{
		[&]<std::size_t... Is>(std::index_sequence<Is...>)
		{
			(void)((Is == n ? (func.template operator()<Is>(), true) : false) || ...);
		}(std::make_index_sequence<MaxN>{});
	}

}
