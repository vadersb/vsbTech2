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


using namespace vsb;

namespace
{
	class TestScopedObject : public ScopedObject
	{
	public:

		explicit TestScopedObject(const int value) : m_value(value)
		{}

		[[nodiscard]] int GetValue() const {return m_value;}

	private:


		int m_value;


	};


	class TestManagedObject : public ManagedObject
	{
	public:

		explicit TestManagedObject(int value) : m_value(value)
		{
			VSBLOG_INFO("TestManagedObject created");
		}


		static SafePtr<TestManagedObject> Create(const int value)
		{
			return new TestManagedObject(value);
		}


		~TestManagedObject() override
		{
			VSBLOG_INFO("TestManagedObject destroyed. value: {}", m_value);
		}


		[[nodiscard]] int GetValue() const {return m_value;}
		void SetValue(const int value) {m_value = value;}

		void Destroy()
		{
			ScheduleForDestruction();
		}

	private:

		int m_value;

	};

}



int main()
{
	VSBInit();

	VSBLOG_INFO("objects in registry: {}", internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

	{
		TestScopedObject t1(123);
		VSBLOG_INFO("objects in registry: {}", internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

		auto hnd1 = CreateHnd(&t1);

		VSBLOG_INFO("hnd1 is valid: {}", hnd1.IsValid());

		if (auto pT1 = hnd1.ValidateAndGet())
		{
			VSBLOG_INFO("pT1 value: {}", pT1->GetValue());
		}

		hnd1.Reset();

		if (auto pT1 = hnd1.ValidateAndGet())
		{
			VSBLOG_INFO("pT1 value: {}", pT1->GetValue());
		}


		TestScopedObject t2(456);
		VSBLOG_INFO("objects in registry: {}", internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

		auto ptr1 = CreatePtr(&t2);

		VSBLOG_INFO("ptr1 is valid: {}", ptr1.IsValid());

		VSBLOG_INFO("ptr1 value: {}", ptr1->GetValue());

		ptr1.Reset();

		TestScopedObject t3(789);
		VSBLOG_INFO("objects in registry: {}", internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());

		auto safePtr1 = CreateSafePtr(&t3);

		SafePtr<Object> baseSafePtr1 = safePtr1;

		VSBLOG_INFO("baseSafePtr1 is valid: {}", baseSafePtr1.IsValid());

		if (safePtr1)
		{
			VSBLOG_INFO("safePtr1 is valid: {}", safePtr1.IsValid());
			VSBLOG_INFO("safePtr1 value: {}", safePtr1->GetValue());
		}

		SafePtr<TestManagedObject> invalidSafePtr(safePtr1);


		safePtr1.Reset();


		VSBLOG_INFO("invalidSafePtr is valid: {}", invalidSafePtr.IsValid());

		VSBLOG_INFO("t1 value: {}", t1.GetValue());
		VSBLOG_INFO("t2 value: {}", t2.GetValue());
		VSBLOG_INFO("t3 value: {}", t3.GetValue());


		const auto stats = internal::ObjectRegistry::GetActiveObjectStats();

		VSBLOG_INFO("unspecified objcts count: {}", stats.unspecifiedObjectsCount);
		VSBLOG_INFO("static objects count: {}", stats.staticObjectsCount);
		VSBLOG_INFO("scoped objects count: {}", stats.scopedObjectsCount);
		VSBLOG_INFO("managed objects count: {}", stats.managedObjectsCount);

	}


	const auto m1 = CreateSafePtr(new TestManagedObject(112233));

	VSBLOG_INFO("m1 value: {}", m1->GetValue());

	m1->SetValue(332211);

	VSBLOG_INFO("m1 value: {}", m1->GetValue());

	m1->Destroy();


	auto m2 = TestManagedObject::Create(445566);

	VSBLOG_INFO("m2 value: {}", m2->GetValue());

	m2->SetValue(665544);

	VSBLOG_INFO("m2 value: {}", m2->GetValue());

	m2->Destroy();

	m2.Reset();


	// const vsb::SafePtr<const TestManagedObject> m3 = TestManagedObject::Create(778899);
	//
	// VSBLOG_INFO("m3 value: {}", m3->GetValue());
	//
	// vsb::SafePtr<TestManagedObject> m3copy = m3;
	//
	// m3copy->Destroy();


	VSBLOG_INFO("objects in registry: {}", vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());


	DestructionCentral::ProcessDefault();


	VSBLOG_INFO("objects in registry: {}", vsb::internal::ObjectRegistry::GetCurrentlyRegisteredObjectsCount());


	VSBLOG_INFO("some extra tests...");

	bool isHndTriviallyCopyable = std::is_trivially_copyable_v<Hnd<TestManagedObject>>;

	VSBLOG_INFO("Hnd<T> is trivially copyable: {}", isHndTriviallyCopyable);

	bool isHndStandardLayout = std::is_standard_layout_v<Hnd<TestManagedObject>>;

	VSBLOG_INFO("Hnd<T> is standard layout: {}", isHndStandardLayout);


	bool isSafePtrTriviallyCopyable = std::is_trivially_copyable_v<SafePtr<TestManagedObject>>;

	VSBLOG_INFO("SafePtr<T> is trivially copyable: {}", isSafePtrTriviallyCopyable);

	bool isSafePtrStandardLayout = std::is_standard_layout_v<SafePtr<TestManagedObject>>;

	VSBLOG_INFO("SafePtr<T> is standard layout: {}", isSafePtrStandardLayout);


	bool isHandleTriviallyCopyable = std::is_trivially_copyable_v<internal::Handle>;

	VSBLOG_INFO("Handle is trivially copyable: {}", isHandleTriviallyCopyable);

	bool isHandleStandardLayout = std::is_standard_layout_v<internal::Handle>;

	VSBLOG_INFO("Handle is standard layout: {}", isHandleStandardLayout);



	//std::string str;

	//std::cin >> str;

	VSBUninit();

	return 0;
}