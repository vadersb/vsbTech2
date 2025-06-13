//(C) 2025 Alexander Samarin

#include <array>
#include <vector>

#include "vsb/log.h"
#include "vsb/debug.h"
#include "vsb/containers/fixed_array.h"
#include "vsb/containers/inplace_stack.h"


int main()
{
	vsb::log::Init(true);


	{
		vsb::InplaceStack<int, 10> stack;
		VSBLOG_INFO("stack size: {}", stack.GetSize());

		stack.Push(1);
		VSBLOG_INFO("stack size: {}", stack.GetSize());

		stack.Push(2);
		VSBLOG_INFO("stack size: {}", stack.GetSize());

		stack.Push(3);
		VSBLOG_INFO("stack size: {}", stack.GetSize());

		auto i = stack.Pop();
		VSBLOG_INFO("stack size: {}. popped: {}", stack.GetSize(), i);

		i = stack.Pop();
		VSBLOG_INFO("stack size: {}. popped: {}", stack.GetSize(), i);

		i = stack.Pop();
		VSBLOG_INFO("stack size: {}. popped: {}", stack.GetSize(), i);


	}


	{
		vsb::FixedArray array {1, 2, 3};

		array.Fill(1);

		VSBLOG_INFO("array size: {}", array.GetSize());

		//array[4] = 123;
	}

	{
		vsb::FixedArray<int, 8> intsArray {1, 2, 3};

		intsArray[5] = 123;

		VSBLOG_INFO("intsArray size: {}", intsArray.GetSize());
	}


	{
		std::array intsArray {1, 2, 3};

		intsArray[0] = 0;

		VSBLOG_INFO("intsArray size: {}", intsArray.size());
	}


	{
		std::vector intsVector {1, 2, 3};

		intsVector[0] = 0;

		VSBLOG_INFO("intsVector size: {}", intsVector.size());
	}



	vsb::log::Uninit();


	return 0;
}