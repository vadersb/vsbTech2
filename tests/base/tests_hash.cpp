//(C) 2025 Alexander Samarin

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "vsb/hash.h"
#include "vsb/log.h"


TEST_CASE("Hash64 - basic tests", "[base][hash]")
{

	SECTION("Empty hash")
	{
		//VSBLOG_INFO("Empty hash test");
		const vsb::Hash64 emptyHash{};
		REQUIRE( emptyHash == vsb::NullHash64 );

		const int i = 0;
		const auto hash = vsb::GetHash64(i);
		REQUIRE( hash == vsb::NullHash64 );
	}

	SECTION("Integer test")
	{
		const int i = 123456;
		const auto hash = vsb::GetHash64(i);
		REQUIRE( hash == 123456 );
	}
}