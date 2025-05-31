//(C) 2025 Alexander Samarin

#pragma once
#include <cstdint>

namespace vsb::internal
{
	class ObjectRegistry;

	struct Handle
	{

		friend class ObjectRegistry;

	public:

		Handle() = default;
		Handle(const uint32_t generation, const uint32_t index) : m_generation(generation), m_index(index)
		{}

		[[nodiscard]] bool IsEmpty() const {return m_generation == 0;}

		bool operator==(const Handle& rhs) const {return m_index == rhs.m_index && m_generation == rhs.m_generation;}

	private:

		uint32_t m_generation {0};
		uint32_t m_index {0};
	};
}
