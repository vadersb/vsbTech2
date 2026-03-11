//
// Created by Alexander on 11.03.2026.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

#include "vsb/strings/string_id.h"

namespace vsb::internal
{
	class StringIDsRegistry
	{

		friend class vsb::StringID;

	private:

		StringIDsRegistry();

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
				return id.GetHash();
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

		std::int32_t RegisterString(std::string_view str, size_t hash);

		inline static StringIDsRegistry* s_pInstance {nullptr};

		static StringIDsRegistry& GetInstance();
		static StringIDsRegistry* GetInstanceInternal() noexcept;


		[[nodiscard]] std::string_view GetString(std::int32_t index) const;

		std::vector<std::string> m_strings {};
		std::unordered_map<StringID, std::int32_t, TransparentHash, TransparentEqual> m_lookupTable {};
	};
}
