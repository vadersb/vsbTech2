//(C) 2025 Alexander Samarin

#pragma once
#include <cstdint>
#include <cstring>
#include <string_view>
#include <bit>
#include <array>

#include "constexpr-xxh3/constexpr-xxh3.h"

namespace vsb
{
	//Hash64 type and default value
	typedef std::uint64_t Hash64;

	inline constexpr static Hash64 NullHash64 = 0;


	//hash calculation
	template<typename T>
	constexpr Hash64 CalculateHash64(const T &v) noexcept
	{
		auto buffer = std::bit_cast<std::array<std::byte, sizeof(T)>>(v);
		return constexpr_xxh3::XXH3_64bits_const(buffer);
	}


	//template struct hash provider
	template<typename Key>
	struct Hash64Provider
	{

	};


	//various template struct specializations
	template<>
	struct Hash64Provider<Hash64>
	{
		constexpr static Hash64 Get(const Hash64& key) noexcept
		{
			return key;
		}
	};


	namespace concepts
	{
		template <typename T>
		concept HasCachedHash64 = requires(const T& obj)
		{
			{ obj.GetCachedHash64() } noexcept -> std::same_as<Hash64>;
		};
	}


	template<concepts::HasCachedHash64 Key>
	struct Hash64Provider<Key>
	{
		constexpr static Hash64 Get(const Key& cachedHash64Holder) noexcept
		{
			return cachedHash64Holder.GetCachedHash64();
		}
	};


	template<std::integral Key>
	struct Hash64Provider<Key>
	{
		constexpr static Hash64 Get(const Key& key)
		{
			if constexpr (sizeof(Key) == sizeof(Hash64))
			{
				return std::bit_cast<Hash64>(key);  // No-op if same size/representation
			}
			else
			{
				if constexpr (sizeof(Key) > sizeof(Hash64))
				{
					return CalculateHash64(key);
				}
				else
				{
					return static_cast<Hash64>(key);
				}
			}
		}
	};


	template<std::convertible_to<std::string_view> Key>
	struct Hash64Provider<Key>
	{
		constexpr static Hash64 Get(const std::string_view& key) noexcept
		{
			return constexpr_xxh3::XXH3_64bits_const(key.data(), key.size());
		}
	};


	template<>
	struct Hash64Provider<const char*>
	{
		constexpr static Hash64 Get(const char* key) noexcept
		{
			return constexpr_xxh3::XXH3_64bits_const(key, std::char_traits<char>::length(key));
		}
	};


	template<typename T>
	constexpr Hash64 GetHash64(const T& v) noexcept
	{
		static_assert(
				requires(const T& key)
				{
					{ Hash64Provider<T>::Get(key) } -> std::convertible_to<Hash64>;
				},
				"Hash64Provider<T> must define a static Get() method that returns Hash64!"
			);

		return Hash64Provider<T>::Get(v);
	}






}