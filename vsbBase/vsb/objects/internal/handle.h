//(C) 2025 Alexander Samarin

#pragma once
#include <cstdint>

namespace vsb
{
	class Object;
	template<typename T> class Hnd;
	template<typename T> class Ptr;
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

	private:

		Handle() = default;

		Handle(const Handle& other) = default;
		Handle& operator=(const Handle& other) = default;

		Handle(Handle&& moveFrom) noexcept
		{
			m_generation = moveFrom.m_generation;
			m_index = moveFrom.m_index;
			moveFrom.m_generation = 0;
			moveFrom.m_index = 0;
		}

		Handle(const uint32_t generation, const uint32_t index) : m_generation(generation), m_index(index)
		{}

		[[nodiscard]] bool IsEmpty() const {return m_generation == 0;}

		bool operator==(const Handle& rhs) const {return m_index == rhs.m_index && m_generation == rhs.m_generation;}



		uint32_t m_generation {0};
		uint32_t m_index {0};
	};
}
