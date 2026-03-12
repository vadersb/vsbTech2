//
// Created by Alexander on 11.03.2026.
//


#include "string_ids_registry.h"
#include "vsb/debug.h"


namespace vsb::internal
{
	StringIDsRegistry::StringIDsRegistry()
	{
		m_strings.reserve(ReserveCapacity);
		m_lookupTable.reserve(ReserveCapacity);
	}


	size_t StringIDsRegistry::GetRegisteredIDsCount() noexcept
	{
		auto* pInstance = GetInstanceIfAvailable();

		if (pInstance == nullptr)
		{
			return 0;
		}

		return pInstance->m_strings.size();
	}


	std::int32_t StringIDsRegistry::RegisterStringInternal(std::string_view str, const size_t hash)
	{
		const TempStringID temp {str, hash};

		if (const auto it = m_lookupTable.find(temp); it != m_lookupTable.end())
		{
			return it->second;
		}

	    auto index = static_cast<std::int32_t>(m_strings.size());
	    VSB_ASSERT(m_strings.size() < static_cast<size_t>(std::numeric_limits<std::int32_t>::max()), "String IDs registry overflow detected");
	    m_strings.emplace_back(str);

		StringID id;
		id.m_cachedHash = hash;
		id.m_index = index;

		m_lookupTable.emplace(id, index);

		return index;
	}


	std::int32_t StringIDsRegistry::RegisterString(std::string_view str, size_t hash)
	{
		auto* pInstance = GetInstanceIfAvailable();

		if (pInstance == nullptr)
		{
			VSB_BREAK;
			return -1;
		}

		return pInstance->RegisterStringInternal(str, hash);
	}


	std::string_view StringIDsRegistry::GetStringInternal(const std::int32_t index) const
	{
	    if (index < 0)
	    {
	        return "";
	    }

	    const size_t finalIndex = static_cast<size_t>(index);
	    VSB_ASSERT(finalIndex < m_strings.size(), "index out of bounds");
	    return m_strings[finalIndex];
	}


	std::string_view StringIDsRegistry::GetString(const std::int32_t index)
	{
		auto* pInstance = GetInstanceIfAvailable();

		if (pInstance == nullptr)
		{
			return "REGISTRY WAS SHUT DOWN";
		}

		return pInstance->GetStringInternal(index);
	}
}
