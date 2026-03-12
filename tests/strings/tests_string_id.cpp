//(C) 2026 Alexander Samarin

#include <catch2/catch_test_macros.hpp>

#include "vsb/strings/string_id.h"
#include "vsb/strings/static_string_id.h"

#include <unordered_set>

namespace
{
    const vsb::StringID someGlobalString {"Some Global String!"};
    constexpr vsb::StaticStringID staticGlobalString {"Some Global String!"};

    const vsb::StringID anotherGlobalString {"Another Global String!"};
}


TEST_CASE("StringID - global strings", "[base][string_id]")
{
    const vsb::StringID globalCopy {"Some Global String!"};

    REQUIRE( someGlobalString.IsEmpty() == false );
    REQUIRE( someGlobalString.GetString() == "Some Global String!" );
    REQUIRE( globalCopy == someGlobalString );

	REQUIRE( staticGlobalString == someGlobalString );
	REQUIRE( staticGlobalString == globalCopy );

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


TEST_CASE("StaticStringID - implicit conversion", "[base][string_id]")
{
	const vsb::StaticStringID staticID{"static_test"};

	const vsb::StringID resolved = staticID;
	REQUIRE_FALSE( resolved.IsEmpty() );
	REQUIRE( resolved.GetString() == "static_test" );
}


TEST_CASE("StaticStringID - caching", "[base][string_id]")
{
	const vsb::StaticStringID staticID{"cache_test"};

	const vsb::StringID first = staticID;
	const vsb::StringID second = staticID;

	REQUIRE( first == second );
}


TEST_CASE("StaticStringID - compatible with StringID", "[base][string_id]")
{
	const vsb::StaticStringID staticID{"compat_test"};
	const vsb::StringID directID{"compat_test"};

	const vsb::StringID fromStatic = staticID;
	REQUIRE( fromStatic == directID );
}


TEST_CASE("StaticStringID - deduplication with registry", "[base][string_id]")
{
	const size_t countBefore = vsb::StringID::GetRegisteredIDsCount();

	const vsb::StaticStringID staticID{"dedup_static"};
	const vsb::StringID directID{"dedup_static"};

	// Trigger the lazy resolution
	const vsb::StringID fromStatic = staticID;

	const size_t countAfter = vsb::StringID::GetRegisteredIDsCount();

	// Only one new entry regardless of creation path
	REQUIRE( countAfter == countBefore + 1 );
	REQUIRE( fromStatic == directID );
}


TEST_CASE("StaticStringID - as function-local static", "[base][string_id]")
{
	auto getId = []() -> vsb::StringID
	{
		static constexpr vsb::StaticStringID id{"local_static_test"};
		return id;
	};

	const vsb::StringID first = getId();
	const vsb::StringID second = getId();

	REQUIRE( first == second );
	REQUIRE( first.GetString() == "local_static_test" );
}


