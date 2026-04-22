//(C) 2025 Alexander Samarin

#include <iostream>

#include "vsb/log.h"
#include "vsb/debug.h"
#include "vsb/delegate.h"


namespace
{
	using TestDelegate = vsb::Delegate<void, int, bool>;

void TestFunc1(int i, bool b)
{
	VSBLOG_INFO("TestFunc1 called with i: {} and b: {}", i, b);
}

void TestLambdaFunc(int i, bool b)
{
	VSBLOG_INFO("TestLambdaFunc called with i: {} and b: {}", i, b);
}


	class SomeTestClass : public vsb::Object
	{
	public:

		SomeTestClass() : vsb::Object(vsb::ObjectHint::Scoped)
		{}

		void TestFunc2(int i, bool b)
		{
			VSBLOG_INFO("TestFunc2 called with i: {} and b: {}", i + m_Value, b);
		}

	private:

		int m_Value {123};

	};



}

int main()
{
	vsb::VSBInit();

	VSBLOG_INFO("Delegate size: {} bytes", sizeof(TestDelegate));

	TestDelegate d1;

	TestDelegate d2(TestFunc1);

	d2(5, true);

	SomeTestClass t1;
	TestDelegate d3(CreateHnd(&t1), &SomeTestClass::TestFunc2);

	d3(123, false);


	TestDelegate d4(vsb::CreateHnd(&t1), &SomeTestClass::TestFunc2);

	bool d3_and_d4_are_equal = d3 == d4;

	VSBLOG_INFO("d3 and d4 are equal: {}", d3_and_d4_are_equal);

	// Simple lambda
	TestDelegate d5([](int i, bool b)
		{VSBLOG_INFO("Lambda called with i: {} and b: {}", i, b);});

	// Lambda with capture (as long as it fits in 32 bytes)
	int capturedValue = 42;
	TestDelegate d6([capturedValue](int i, bool b)
		{VSBLOG_INFO("Lambda with capture: {} called with i: {} and b: {}", capturedValue, i, b);});

	d5(10, true);
	d6(20, false);

	bool d5_and_d6_are_equal = d5 == d6;

	VSBLOG_INFO("d5 and d6 are equal: {}", d5_and_d6_are_equal);

	{
		TestDelegate autoLambdaDelegate([](auto i, auto b) {TestLambdaFunc(i, b);});
		autoLambdaDelegate(15, true);

		TestDelegate autoLambdaDelegate2([](auto i, auto b) {VSBLOG_INFO("autoLambdaDelegate2 is called with {} and {}", i, b);});
		autoLambdaDelegate2(25, false);
	}



	auto d7 = d5;

	d7(30, true);

	bool d5_and_d7_are_equal = d5 == d7;

	VSBLOG_INFO("d5 and d7 are equal: {}", d5_and_d7_are_equal);


	uint64_t value2 = 69;
	uint64_t value3 = 123;
	uint64_t value4 = 456;

	TestDelegate d8([capturedValue, value2, value3](int i, bool b)
		{VSBLOG_INFO("Captured values: {}, {}, {}, Lambda called with i: {} and b: {}", capturedValue, value2, value3, i, b);});

	d8(10, false);

	vsb::VSBUninit();

	return 0;
}