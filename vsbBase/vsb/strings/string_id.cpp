//
// Created by Alexander on 11.03.2026.
//

#include "string_id.h"
#include "internal/string_ids_registry.h"

namespace vsb
{
	StringID::StringID(const std::string_view str)
	{
	    m_cachedHash = std::hash<std::string_view>{}(str);
	    m_index = internal::StringIDsRegistry::RegisterString(str, m_cachedHash);
	}


	std::string_view StringID::GetString() const noexcept
	{
		if (m_index == -1)
		{
			return "";
		}

		return internal::StringIDsRegistry::GetString(m_index);
	}


	size_t StringID::GetRegisteredIDsCount() noexcept
	{
		return internal::StringIDsRegistry::GetRegisteredIDsCount();
	}
}
