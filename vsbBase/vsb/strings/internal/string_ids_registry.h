//
// Created by Alexander on 11.03.2026.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

#include "vsb/strings/string_id.h"
#include "vsb/objects/standalone_singleton.h"

namespace vsb::internal
{
	class StringIDsRegistry : public StandaloneSingleton<StringIDsRegistry, false>
	{
		constexpr static size_t ReserveCapacity = 1024 * 16;

		friend class vsb::StringID;
		template<typename TSingleton, bool publicInstanceAccess> friend class vsb::StandaloneSingleton;

		StringIDsRegistry();
		~StringIDsRegistry() = default;


		static size_t GetRegisteredIDsCount() noexcept;

		struct TempStringID
		{
			std::string_view m_str;
			size_t m_hash;
		};

		struct TransparentHash
		{
			using is_transparent = void;

			size_t operator()(const StringID& id) const noexcept
			{
				return id.m_cachedHash;
			}

			size_t operator()(const TempStringID& tmp) const noexcept
			{
				return tmp.m_hash;
			}
		};

		struct TransparentEqual
		{
			using is_transparent = void;

			bool operator()(const StringID& lhs, const StringID& rhs) const noexcept
			{
				return lhs == rhs;
			}

			bool operator()(const TempStringID& lhs, const StringID& rhs) const noexcept
			{
				return lhs.m_str == rhs.GetString();
			}

			bool operator()(const StringID& lhs, const TempStringID& rhs) const noexcept
			{
				return lhs.GetString() == rhs.m_str;
			}
		};

		std::int32_t RegisterStringInternal(std::string_view str, size_t hash);
		static std::int32_t RegisterString(std::string_view str, size_t hash);



		[[nodiscard]] std::string_view GetStringInternal(std::int32_t index) const;
		static std::string_view GetString(std::int32_t index);

		std::vector<std::string> m_strings {};
		std::unordered_map<StringID, std::int32_t, TransparentHash, TransparentEqual> m_lookupTable {};
	};
}
