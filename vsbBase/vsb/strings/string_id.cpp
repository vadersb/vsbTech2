//
// Created by Alexander on 11.03.2026.
//

#include "string_id.h"
#include "internal/string_ids_registry.h"

namespace vsb
{
	std::hash<std::string_view> s_StringViewHash;

	StringID::StringID(const std::string_view str)
	{
		m_cachedHash = s_StringViewHash(str);
		m_index = internal::StringIDsRegistry::GetInstance().RegisterString(str, m_cachedHash);
	}


	std::string_view StringID::GetString() const noexcept
	{
		if (m_index == -1)
		{
			return "";
		}

		return internal::StringIDsRegistry::GetInstance().GetString(m_index);
	}
}
