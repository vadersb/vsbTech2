//(C) 2025 Alexander Samarin

#include "catch2/catch_test_macros.hpp"
#include "vsb/log.h"
#include "vsb/objects/singleton.h"

namespace
{
	class BasicSingleton : public vsb::Singleton<BasicSingleton>
	{
	public:

		BasicSingleton()
		{
			s_Counter++;
		}

		static constexpr int DefaultValue = 0;

		[[nodiscard]] int GetValue() const {return m_value;}
		void SetValue(int value) {m_value = value;}
		void Reset() {m_value = DefaultValue;}
		void Increment() {m_value++;}

		static int GetCounter() {return s_Counter;}

	private:


		inline static int s_Counter = 0;

		int m_value = DefaultValue;
	};
}

TEST_CASE("Singleton - basic tests", "[objects][singleton]")
{
	SECTION("Singleton init and get default value")
	{
		auto defaultValue = BasicSingleton::GetInstance().GetValue();

		REQUIRE(defaultValue == BasicSingleton::DefaultValue);
	}

	const int SomeValue = 123;

	SECTION("Singleton set some value")
	{
		BasicSingleton::GetInstance().SetValue(SomeValue);

		REQUIRE(BasicSingleton::GetInstance().GetValue() == SomeValue);
	}


	SECTION("Singleton retains value")
	{
		REQUIRE(BasicSingleton::GetInstance().GetValue() == SomeValue);
	}


	SECTION("Singleton reset value")
	{
		BasicSingleton::GetInstance().SetValue(456);
		BasicSingleton::GetInstance().Increment();

		BasicSingleton::GetInstance().Reset();

		REQUIRE(BasicSingleton::GetInstance().GetValue() == BasicSingleton::DefaultValue);
	}


	SECTION("Singleton instance counter")
	{
		REQUIRE(BasicSingleton::GetCounter() == 1);
	}
}