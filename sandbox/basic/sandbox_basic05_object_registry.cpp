//(C) 2025 Alexander Samarin

#include <iostream>

#include "vsb/log.h"
#include "vsb/debug.h"
#include "vsb/objects/hnd.h"
#include "vsb/objects/managed_object.h"
#include "vsb/objects/object.h"
#include "vsb/objects/ptr.h"
#include "vsb/objects/safe_ptr.h"
#include "vsb/objects/internal/object_registry.h"


namespace
{
	class TestScopedObject : public vsb::Object
	{
	public:

		explicit TestScopedObject(const int value) : vsb::Object(vsb::ObjectHint::Scoped), m_value(value)
		{}

		[[nodiscard]] int GetValue() const {return m_value;}

	private:


		int m_value;


	};


	class TestManagedObject : public vsb::ManagedObject<>
	{

	};
}



int main()
{
	vsb::internal::ObjectRegistryFinalizer objectRegistryFinalizer;

	VSBLOG_INFO("objects in registry: {}", (vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount()));

	{
		TestScopedObject t1(123);
		VSBLOG_INFO("objects in registry: {}", (int)vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

		auto hnd1 = vsb::CreateHnd(&t1);

		VSBLOG_INFO("hnd1 is valid: {}", hnd1.IsValid());

		if (auto pT1 = hnd1.ValidateAndGet(); pT1 != nullptr)
		{
			VSBLOG_INFO("pT1 value: {}", pT1->GetValue());
		}

		hnd1.Reset();

		TestScopedObject t2(456);
		VSBLOG_INFO("objects in registry: {}", (int)vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

		auto ptr1 = vsb::CreatePtr(&t2);

		VSBLOG_INFO("ptr1 is valid: {}", ptr1.IsValid());

		VSBLOG_INFO("ptr1 value: {}", ptr1->GetValue());

		ptr1.Reset();

		TestScopedObject t3(789);
		VSBLOG_INFO("objects in registry: {}", vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

		auto safePtr1 = vsb::CreateSafePtr(&t3);

		if (safePtr1)
		{
			VSBLOG_INFO("safePtr1 is valid: {}", safePtr1.IsValid());
			VSBLOG_INFO("safePtr1 value: {}", safePtr1->GetValue());
		}

		safePtr1.Reset();



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