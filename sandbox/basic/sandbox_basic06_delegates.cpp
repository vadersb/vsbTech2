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


	class SomeTestClass : public vsb::Object
	{
	public:

		SomeTestClass() : vsb::Object(vsb::ObjectHint::Scoped)
		{}

		void TestFunc2(int i, bool b)
		{
			VSBLOG_INFO("TestFunc2 called with i: {} and b: {}", i, b);
		}


	};



}

int main()
{
	TestDelegate d1;

	TestDelegate d2(TestFunc1);

	d2(5, true);

	VSBLOG_INFO("Delegate size: {}", sizeof(TestDelegate));


	SomeTestClass t1;
	TestDelegate d3(CreateHnd(&t1), &SomeTestClass::TestFunc2);

	d3(123, false);


	TestDelegate d4(vsb::CreateHnd(&t1), &SomeTestClass::TestFunc2);

	bool d3_and_d4_are_equal = d3 == d4;

	VSBLOG_INFO("d3 and d4 are equal: {}", d3_and_d4_are_equal);


	return 0;
}


