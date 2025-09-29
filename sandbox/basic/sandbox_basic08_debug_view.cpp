//
// Created by Alexander on 29.09.2025.
//


#include "vsb/log.h"
#include "vsb/objects/object.h"
#include "vsb/objects/safe_ptr.h"


namespace
{
	class BaseInterface
	{
	public:

		[[nodiscard]] virtual int GetValue() const = 0;

	protected:

		virtual ~BaseInterface() = default;
	};


	class Base : public vsb::Object, virtual public BaseInterface
	{
	public:

		[[nodiscard]] int GetValue() const override
		{
			return m_Value;
		}

	protected:

		explicit Base(int value) :
		vsb::Object(vsb::ObjectHint::Scoped),
		m_Value(value)
		{}


	private:

		int m_Value;
	};


	class BasicInherited : public Base
	{
	public:

		explicit BasicInherited(int intValue = 0, float floatValue = 0.0f) : Base(intValue), m_FloatValue(floatValue)
		{
		}

	private:

		float m_FloatValue;
	};


	class ExtendedInterface : virtual public BaseInterface
	{
	public:

		[[nodiscard]] virtual std::string_view GetString() const = 0;

	protected:
		~ExtendedInterface() override = default;
	};


	class ExtendedInherited : public Base, public ExtendedInterface
	{
	public:

		[[nodiscard]] std::string_view GetString() const override
		{
			return m_String;
		}


		explicit ExtendedInherited(std::string_view stringValue = "", int intValue = 0) :
		Base(intValue), m_String(stringValue)
		{}


	private:

		std::string m_String;
	};


}





int main()
{
	VSBLOG_INFO("Debug viewing testing");

	BasicInherited b1(123);
	BasicInherited b2(456);

	ExtendedInherited ex1("tra la la", 111);
	ExtendedInherited ex2("okokok!", 333);


	vsb::SafePtr<Base> p1(&b1);
	vsb::SafePtr<Base> p2(&b2);
	vsb::SafePtr<Base> p3(&ex1);
	vsb::SafePtr<Base> p4(&ex2);

	std::vector<vsb::SafePtr<Base>> vec;

	vec.push_back(p1);
	vec.push_back(p2);
	vec.push_back(p3);
	vec.push_back(p4);

	VSBLOG_INFO("is valid: {}", vec[3].IsValid());

}
