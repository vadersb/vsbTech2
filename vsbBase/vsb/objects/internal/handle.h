//(C) 2025 Alexander Samarin

#pragma once
#include <cstdint>
#include "vsb/hash.h"

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

		friend class vsb::Object;
		friend class ObjectRegistry;
		template<typename T> friend class vsb::Hnd;
		template<typename T> friend class vsb::Ptr;
		template<typename T> friend class vsb::SafePtr;

		[[nodiscard]] Hash64 GetHash64() const
		{
			const std::array hashValues = {m_generation, m_index};
			return std::bit_cast<Hash64>(hashValues);
		}

	private:

		static const Handle Empty;

		Handle() = default;

		Handle(const Handle& other) = default;
		Handle& operator=(const Handle& other) = default;

		//custom move constructor was removed to preserve Handle as trivially copyable type

		Handle(const uint32_t generation, const uint32_t index) : m_generation(generation), m_index(index)
		{}

		[[nodiscard]] bool IsEmpty() const {return m_generation == 0;}

		bool operator==(const Handle& rhs) const {return m_index == rhs.m_index && m_generation == rhs.m_generation;}



		uint32_t m_generation {0};
		uint32_t m_index {0};
	};

	static_assert(std::is_trivially_copyable_v<Handle>);
	static_assert(std::is_standard_layout_v<Handle>);
}
