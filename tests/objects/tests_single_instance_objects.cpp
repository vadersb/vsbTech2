//(C) 2025 Alexander Samarin

#include "catch2/catch_test_macros.hpp"
#include "vsb/log.h"
#include "vsb/objects/single_instance_object.h"

namespace
{
	class TestSingleInstanceObject : public vsb::SingleInstanceObject<TestSingleInstanceObject>
	{
	public:

		explicit TestSingleInstanceObject(int value) : m_value(value)
		{}

		[[nodiscard]] int GetValue() const {return m_value;}
		void SetValue(const int value) {m_value = value;}

	private:

		int m_value {};
	};
}


TEST_CASE("SingleInstanceObject tests", "[objects][single instance objects]")
{
	SECTION("Before instance creation")
	{
		auto pInstance = TestSingleInstanceObject::GetCurrentInstancePtr();
		REQUIRE(pInstance == nullptr);
	}

	SECTION("Instance creation")
	{
		new TestSingleInstanceObject(123);

		auto pInstance = TestSingleInstanceObject::GetCurrentInstancePtr();
		REQUIRE(pInstance != nullptr);
	}

	SECTION("Instance usage")
	{
		const int curValue = TestSingleInstanceObject::GetCurrentInstance().GetValue();
		REQUIRE(curValue == 123);
	}


	SECTION("Instance destruction")
	{
		//uncommenting this line should lead to an error
		//new TestSingleInstanceObject(456);

		auto pInstance = TestSingleInstanceObject::GetCurrentInstancePtr();
		REQUIRE(pInstance != nullptr);

		delete pInstance;
	}


	SECTION("After instance destruction")
	{
		auto pInstance = TestSingleInstanceObject::GetCurrentInstancePtr();
		REQUIRE(pInstance == nullptr);
	}


	SECTION("Another instance creation")
	{
		new TestSingleInstanceObject(456);
	}


	SECTION("Instance usage")
	{
		const int curValue = TestSingleInstanceObject::GetCurrentInstance().GetValue();
		REQUIRE(curValue == 456);
	}


	SECTION("Instance destruction")
	{
		auto pInstance = TestSingleInstanceObject::GetCurrentInstancePtr();
		REQUIRE(pInstance != nullptr);
		delete pInstance;
	}


	SECTION("After another instance destruction")
	{
		auto pInstance = TestSingleInstanceObject::GetCurrentInstancePtr();
		REQUIRE(pInstance == nullptr);
	}
}