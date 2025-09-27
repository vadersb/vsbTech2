//
// Created by Alexander on 27.09.2025.
//

#pragma once


#include <type_traits>
//#include <utility> // for std::declval

namespace vsb
{
	namespace internal
	{
		// Helper trait to check if static_cast is possible
		template <typename From, typename To, typename = void>
		struct can_static_cast : std::false_type {};

		template <typename From, typename To>
		struct can_static_cast<From, To, std::void_t<decltype(static_cast<To>(std::declval<From>()))>> : std::true_type {};

		// Trait to check if Base is a virtual base of Derived
		template <typename Base, typename Derived>
		struct is_virtual_base_of : std::conjunction<
			std::is_base_of<Base, Derived>,
			std::negation<can_static_cast<Base*, Derived*>>
		> {};
	}


	// Concept using the trait
	template <typename Base, typename Derived>
	concept VirtualInheritance = internal::is_virtual_base_of<Base, Derived>::value;


	namespace internal::tests::virtual_inheritance
	{
		class Base {};
		class Derived : public Base {};
		class DerivedVirtual : public virtual Base {};

		static_assert(VirtualInheritance<Base, Derived> == false);
		static_assert(VirtualInheritance<Base, DerivedVirtual> == true);
	}
}