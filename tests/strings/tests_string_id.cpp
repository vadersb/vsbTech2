//(C) 2026 Alexander Samarin

#include <catch2/catch_test_macros.hpp>

#include "vsb/strings/string_id.h"

#include <unordered_set>

namespace
{
    const vsb::StringID someGlobalString {"Some Global String!"};
    const vsb::StringID anotherGlobalString {"Another Global String!"};
}


TEST_CASE("StringID - global strings", "[base][string_id]")
{
    const vsb::StringID globalCopy {"Some Global String!"};

    REQUIRE( someGlobalString.IsEmpty() == false );
    REQUIRE( someGlobalString.GetString() == "Some Global String!" );
    REQUIRE( globalCopy == someGlobalString );

    REQUIRE( anotherGlobalString.IsEmpty() == false );
    REQUIRE( anotherGlobalString.GetString() == "Another Global String!" );
    REQUIRE_FALSE( someGlobalString == anotherGlobalString );
}


TEST_CASE("StringID - default construction", "[base][string_id]")
{
	const vsb::StringID empty{};
	REQUIRE( empty.IsEmpty() );
	REQUIRE( empty.GetString() == "" );
}


TEST_CASE("StringID - construction from string", "[base][string_id]")
{
	const vsb::StringID id{"hello"};
	REQUIRE_FALSE( id.IsEmpty() );
	REQUIRE( id.GetString() == "hello" );
}


TEST_CASE("StringID - equality", "[base][string_id]")
{
	SECTION("Same string produces equal IDs")
	{
		const vsb::StringID a{"world"};
		const vsb::StringID b{"world"};
		REQUIRE( a == b );
	}

	SECTION("Different strings produce different IDs")
	{
		const vsb::StringID a{"alpha"};
		const vsb::StringID b{"beta"};
		REQUIRE_FALSE( a == b );
	}

	SECTION("Two default-constructed IDs are equal")
	{
		const vsb::StringID a{};
		const vsb::StringID b{};
		REQUIRE( a == b );
	}
}


TEST_CASE("StringID - string retrieval", "[base][string_id]")
{
	const vsb::StringID id{"test_string"};
	REQUIRE( id.GetString() == "test_string" );

	// Retrieve again to ensure stability
	REQUIRE( id.GetString() == "test_string" );
}


TEST_CASE("StringID - deduplication", "[base][string_id]")
{
	const size_t countBefore = vsb::StringID::GetRegisteredIDsCount();

	const vsb::StringID first{"duplicate_me"};
	const size_t countAfterFirst = vsb::StringID::GetRegisteredIDsCount();
	REQUIRE( countAfterFirst == countBefore + 1 );

	const vsb::StringID second{"duplicate_me"};
	const size_t countAfterSecond = vsb::StringID::GetRegisteredIDsCount();
	REQUIRE( countAfterSecond == countAfterFirst );

	REQUIRE( first == second );
}


TEST_CASE("StringID - std::hash support", "[base][string_id]")
{
	const vsb::StringID a{"hash_me"};
	const vsb::StringID b{"hash_me"};

	const std::hash<vsb::StringID> hasher{};
	REQUIRE( hasher(a) == hasher(b) );

	SECTION("Usable in unordered containers")
	{
		std::unordered_set<vsb::StringID> set;
		set.insert(vsb::StringID{"one"});
		set.insert(vsb::StringID{"two"});
		set.insert(vsb::StringID{"one"});

		REQUIRE( set.size() == 2 );
	}
}


TEST_CASE("StringID - trivially copyable", "[base][string_id]")
{
	const vsb::StringID original{"copy_me"};
	const vsb::StringID copy = original;

	REQUIRE( copy == original );
	REQUIRE( copy.GetString() == "copy_me" );

	vsb::StringID anotherCopy;

	std::memcpy(&anotherCopy, &original, sizeof(vsb::StringID));

	REQUIRE(anotherCopy.GetString() == "copy_me");
}


