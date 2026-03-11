//
// Created by Alexander on 11.03.2026.
//


#include "string_ids_registry.h"


namespace vsb::internal
{


	StringIDsRegistry::StringIDsRegistry()
	{
		m_strings.reserve(1024);
		m_lookupTable.reserve(1024);
	}


	std::int32_t StringIDsRegistry::RegisterString(std::string_view str, size_t hash)
	{
		const TempStringID temp {str, hash};

		if (const auto it = m_lookupTable.find(temp); it != m_lookupTable.end())
		{
			return it->second;
		}

		auto index = static_cast<std::int32_t>(m_strings.size());
		m_strings.emplace_back(str);

		StringID id;
		id.m_cachedHash = hash;
		id.m_index = index;

		m_lookupTable.emplace(id, index);

		return index;
	}


	StringIDsRegistry& StringIDsRegistry::GetInstance()
	{
		if (s_pInstance == nullptr)
		{
			s_pInstance = GetInstanceInternal();
		}

		return *s_pInstance;
	}


	StringIDsRegistry* StringIDsRegistry::GetInstanceInternal() noexcept
	{
		static StringIDsRegistry staticInstance;
		return &staticInstance;
	}


	std::string_view StringIDsRegistry::GetString(const std::int32_t index) const
	{
		if (index < 0)
		{
			return "";
		}

		const size_t finalIndex = static_cast<size_t>(index);
		return m_strings[finalIndex];
	}
}
