//(C) 2025 Alexander Samarin

#include "vsb/log.h"
#include "vsb/debug.h"
#include "vsb/hash.h"


namespace
{
	class TestClass
	{
	public:

		TestClass(int value1, int value2, int value3) : m_value1(value1), m_value2(value2), m_value3(value3), m_Hash(CalculateHash())
		{

		}

		[[nodiscard]] vsb::Hash64 GetCachedHash64() const noexcept {return m_Hash;}

	private:

		[[nodiscard]] vsb::Hash64 CalculateHash() const
		{
			// auto h1 = vsb::GetHash64(m_value1);
			// auto h2 = vsb::GetHash64(m_value2);
			//
			// auto h = h1 ^ h2;

			auto h = vsb::CalculateHash64(std::tuple{m_value1, m_value2, m_value3});

			return h;
		}

		int m_value1;
		int m_value2;
		int m_value3;

		vsb::Hash64 m_Hash;
	};
}



int main()
{
	vsb::log::Init(true);

	vsb::log::SetLevel(vsb::log::Level::Trace);


	vsb::Hash64 h1 = vsb::NullHash64;
	VSB_ASSERT(h1 == 0, "");

	const auto h2 = vsb::GetHash64(123);

	VSBLOG_INFO("h2: {}", static_cast<uint64_t>(h2));


	const auto h3 = vsb::GetHash64(char('t'));

	VSBLOG_INFO("h3: {}", static_cast<uint64_t>(h3));


	const auto h4 = vsb::GetHash64(true);

	VSBLOG_INFO("h4: {}", static_cast<uint64_t>(h4));


	std::string s1 = "hello world";
	const auto h5 = vsb::GetHash64(s1);

	VSBLOG_INFO("h5: {}", static_cast<uint64_t>(h5));


	const auto h6 = vsb::GetHash64("hello world");

	VSBLOG_INFO("h6: {}", static_cast<uint64_t>(h6));


	TestClass t1(123, 456, 789);
	const auto h7 = vsb::GetHash64(t1);
	VSBLOG_INFO("h7: {}", static_cast<uint64_t>(h7));




}