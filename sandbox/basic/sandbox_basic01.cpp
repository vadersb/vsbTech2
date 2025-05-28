//(C) 2025 Alexander Samarin

#include "vsb/log.h"
#include "vsb/debug.h"

int main()
{
	vsb::log::Init(true);

	vsb::log::SetLevel(vsb::log::Level::Trace);

	VSBLOG_TRACE("this is trace. integer: {}", 123);

	VSBLOG_DEBUG("this is debug. integer: {}", 234);

	VSBLOG_INFO("this is info. integer: {}", 456);

	VSBLOG_WARN("this is warn. integer: {}", 567);

	VSBLOG_ERROR("this is error. integer: {}", 678);

	VSBLOG_CRITICAL("this is critical. integer: {}", 789);


	int i = 123;

	VSB_ASSERT_V(i % 2 == 0, "int should be even", i + 5);


	VSBLOG_INFO("wrap up");

	vsb::log::Uninit();
}