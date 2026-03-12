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
