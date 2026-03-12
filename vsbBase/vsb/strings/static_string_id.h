//
// Created by Alexander on 12.03.2026.
//

#pragma once

#include <string>
#include "string_id.h"


namespace vsb
{
	class StaticStringID
	{
	public:

		consteval explicit StaticStringID(const std::string_view str) : m_String(str)
		{

		}

		// ReSharper disable once CppNonExplicitConversionOperator
		operator StringID() const noexcept
		{
			if (!m_isCached) [[unlikely]]
			{
				m_cachedID = StringID(m_String);
				m_isCached = true;
			}

			return m_cachedID;
		}


		operator std::string_view() const noexcept
		{
			return m_String;
		}


		bool operator ==(const StringID& stringID) const
		{
			const StringID id = *this;
			return stringID == id;
		}


	private:

		std::string_view m_String;

		mutable bool m_isCached {false};
		mutable StringID m_cachedID {};
	};
}



//TO ADD LATER:
// template<std::size_t N>
// struct FixedString
// {
// 	char data[N]{};
// 	std::size_t len{0};
//
// 	consteval FixedString() = default;
//
// 	consteval FixedString(const char (&str)[N])
// 	{
// 		for (std::size_t i = 0; i < N; ++i)
// 			data[i] = str[i];
// 		len = N - 1;
// 	}
//
// 	consteval operator std::string_view() const { return {data, len}; }
//
// 	template<std::size_t M>
// 	consteval auto operator+(const FixedString<M>& other) const
// 	{
// 		FixedString<N + M - 1> result;
// 		for (std::size_t i = 0; i < len; ++i)
// 			result.data[i] = data[i];
// 		for (std::size_t i = 0; i < other.len; ++i)
// 			result.data[len + i] = other.data[i];
// 		result.data[len + other.len] = '\0';
// 		result.len = len + other.len;
// 		return result;
// 	}
// };
//
// template<std::size_t N>
// FixedString(const char (&)[N]) -> FixedString<N>;


//USAGE:
// constexpr auto prefix = FixedString("System.");
// constexpr auto name   = FixedString("Renderer");
// constexpr auto full   = prefix + name; // "System.Renderer"
//
// static constexpr StaticStringID id{static_cast<std::string_view>(full)};