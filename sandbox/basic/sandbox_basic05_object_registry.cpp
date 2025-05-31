//(C) 2025 Alexander Samarin

#include <iostream>

#include "vsb/log.h"
#include "vsb/debug.h"
#include "vsb/objects/object.h"
#include "vsb/objects/internal/object_registry.h"


namespace
{
	class TestScopedObject : public vsb::Object
	{
	public:

		explicit TestScopedObject(const int value) : vsb::Object(vsb::ObjectHint::Scoped), m_value(value)
		{}

		int GetValue() const {return m_value;}

	private:


		int m_value;


	};
}



int main()
{
	vsb::internal::ObjectRegistryFinalizer objectRegistryFinalizer;


	VSBLOG_INFO("objects in registry: {}", vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

	{
		TestScopedObject t1(123);
		VSBLOG_INFO("objects in registry: {}", vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

		TestScopedObject t2(456);
		VSBLOG_INFO("objects in registry: {}", vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

		TestScopedObject t3(789);
		VSBLOG_INFO("objects in registry: {}", vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

		VSBLOG_INFO("t1 value: {}", t1.GetValue());
		VSBLOG_INFO("t2 value: {}", t2.GetValue());
		VSBLOG_INFO("t3 value: {}", t3.GetValue());


		const auto stats = vsb::internal::ObjectRegistry::GetActiveObjectStats();

		VSBLOG_INFO("unspecified objcts count: {}", stats.unspecifiedObjectsCount);
		VSBLOG_INFO("static objects count: {}", stats.staticObjectsCount);
		VSBLOG_INFO("scoped objects count: {}", stats.scopedObjectsCount);
		VSBLOG_INFO("managed objects count: {}", stats.managedObjectsCount);

	}

	VSBLOG_INFO("objects in registry: {}", vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

	//std::string str;

	//std::cin >> str;

	return 0;
}