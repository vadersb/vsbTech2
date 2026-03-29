//(C) 2025 Alexander Samarin

#pragma once
#include "vsb/hash.h"
#include <cstdint>

namespace vsb
{
	class Object;
	template<typename T> class Hnd;
	template<typename T> class Ptr;
	template<typename T> class SafePtr;
}

namespace vsb::internal
{
	class ObjectRegistry;

	struct Handle
	{
		Handle() = default;

		~Handle() = default;

		Handle(const Handle& other) = default;
		Handle(Handle&& other) = default;
		Handle& operator=(const Handle& other) = default;
		Handle& operator=(Handle&& other) = default;

		friend class vsb::Object;
		friend class ObjectRegistry;
		template<typename T> friend class vsb::Hnd;
		template<typename T> friend class vsb::Ptr;
		template<typename T> friend class vsb::SafePtr;

		[[nodiscard]] Hash64 GetHash64() const
		{
			const std::array hashValues = {m_index, m_generation};
			return std::bit_cast<Hash64>(hashValues);
		}

	private:

		static const Handle Empty;




		//custom move constructor was removed to preserve Handle as a trivially copyable type

		Handle(const uint32_t index, const uint32_t generation) : m_index(index), m_generation(generation) // NOLINT(*-easily-swappable-parameters)
		{}

		[[nodiscard]] bool IsEmpty() const {return m_generation == 0;}

		bool operator==(const Handle& rhs) const {return m_index == rhs.m_index && m_generation == rhs.m_generation;}

		auto operator<=>(const Handle& rhs) const = default;

		uint32_t m_index {0};       // Primary sort key (slot)
		uint32_t m_generation {0};  // Secondary sort key (version)
	};

	static_assert(std::is_trivially_copyable_v<Handle>);
	static_assert(std::is_standard_layout_v<Handle>);
}
