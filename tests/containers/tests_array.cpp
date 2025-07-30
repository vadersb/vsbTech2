//(C) 2025 Alexander Samarin

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "vsb/hash.h"
#include "vsb/log.h"
#include "vsb/containers/array.h"

TEST_CASE("Containers - Array", "[containers][array]")
{

	SECTION("Empty Array")
	{
		const vsb::Array<int> intsArray;

		REQUIRE(intsArray.GetSize() == 0);
		REQUIRE(intsArray.GetCapacity() == 0);


		const vsb::Array<float> floatsArray(vsb::CapacitySetting(0));

		REQUIRE(floatsArray.GetSize() == 0);
		REQUIRE(floatsArray.GetCapacity() == 0);


		const vsb::Array<uint8_t> bytesArray(vsb::CapacitySetting(32));

		REQUIRE(bytesArray.GetSize() == 0);
		REQUIRE(bytesArray.GetCapacity() == 32);

		const vsb::Array<uint8_t> bytesArray2 {vsb::CapacitySetting(0)};

		REQUIRE(bytesArray2.GetSize() == 0);
		REQUIRE(bytesArray2.GetCapacity() == 0);
	}


	SECTION("Copy Constructors")
	{
		{
			vsb::Array<int> intsArray(vsb::CapacitySetting(32));

			vsb::Array<uint64_t, vsb::memory::AllocationStrategy::StdChecked> longIntsArray(intsArray);

			REQUIRE(longIntsArray.GetSize() == 0);
			REQUIRE(longIntsArray.GetCapacity() == intsArray.GetCapacity());
		}
	}


	SECTION("Adding elements")
	{
		vsb::Array<int> intsArray(vsb::CapacitySetting(32));
		REQUIRE(intsArray.GetSize() == 0);

		intsArray.Add(123);
		REQUIRE(intsArray.GetSize() == 1);

		intsArray.Add(456);
		REQUIRE(intsArray.GetSize() == 2);
	}


	SECTION("Regular Capacity Growth")
	{
		vsb::Array<int> intsArray;

		constexpr auto MinStartCapacity = decltype(intsArray)::MinStartCapacity;

		REQUIRE(intsArray.GetSize() == 0);
		REQUIRE(intsArray.GetCapacity() == 0);

		intsArray.Add(123);

		REQUIRE(intsArray.GetSize() == 1);
		REQUIRE(intsArray.GetCapacity() == MinStartCapacity);

		for (int i = 0; i < MinStartCapacity; i++)
		{
			intsArray.Add(i);
		}

		REQUIRE(intsArray.GetSize() == 9);
		REQUIRE(intsArray.GetCapacity() == MinStartCapacity * 2);
	}


	SECTION("Access")
	{
		vsb::Array<int> intsArray;

		for (int i = 0; i < 16; i++)
		{
			intsArray.Add(i);
		}


		REQUIRE(intsArray[0] == 0);
		REQUIRE(intsArray[vsb::FromEnd(1)] == 15);
	}

}