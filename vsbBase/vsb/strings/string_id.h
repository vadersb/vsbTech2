//
// Created by Alexander on 11.03.2026.
//

#pragma once
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace vsb
{
	class StringID;
}

template<>
struct std::hash<vsb::StringID>;

namespace vsb
{
	namespace internal
	{
		class StringIDsRegistry;
	}

	class StringID
	{
		friend class internal::StringIDsRegistry;
		friend struct std::hash<StringID>;

	public:

		StringID() = default;

		explicit StringID(std::string_view str);

		[[nodiscard]] bool IsEmpty() const noexcept {return m_index == -1;}


		bool operator==(const StringID& other) const noexcept {return m_index == other.m_index;}

		[[nodiscard]] std::string_view GetString() const noexcept;

	private:

		[[nodiscard]] size_t GetHash() const noexcept {return m_cachedHash;}

		size_t m_cachedHash {0};
		std::int32_t m_index {-1};
	};


	static_assert(std::is_trivially_copyable_v<StringID>, "StringID must be trivially copyable");
	static_assert(std::is_standard_layout_v<StringID>, "StringID must be standard layout");
}


template<>
struct std::hash<vsb::StringID>
{
	size_t operator()(const vsb::StringID &stringID) const noexcept
	{
		return stringID.GetHash();
	}
};
