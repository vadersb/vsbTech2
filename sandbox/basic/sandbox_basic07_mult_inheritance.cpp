//
// Created by Alexander on 27.09.2025.
//


#include "vsb/log.h"
#include "vsb/objects/hnd.h"
#include "vsb/objects/safe_ptr.h"

namespace
{
	class IItemView
	{
	public:

		virtual void DoBaseItemStuff(int x, int y) = 0;
		virtual ~IItemView() = default;

	};

	class Item : public vsb::Object
	{
	public:

		Item() : vsb::Object(vsb::ObjectHint::Scoped)
		{}


		void SetPosition(int x, int y)
		{
			m_X = x;
			m_Y = y;
		}

		void DoBaseItemStuff()
		{
			if (auto* pView = m_Ptr.ValidateAndGet())
			{
				pView->DoBaseItemStuff(m_X, m_Y);
			}
		}

		void RegisterView(IItemView* pView)
		{
			m_Ptr = pView;
		}

		void UnregisterView()
		{
			m_Ptr.Reset();
		}


	protected:

		template<typename T>
		vsb::SafePtr<T> GetView()
		{
			//return m_Ptr;
			return vsb::SafePtr<T>(m_Ptr);
		}


	private:

		vsb::SafePtr<IItemView> m_Ptr {};

		int m_X {};
		int m_Y {};
	};


	class IDerivedItemView : public virtual IItemView
	{
	public:

		virtual void DoDerivedItemStuff(std::string_view stringValue) = 0;
	};


	static_assert(vsb::VirtualInheritance<IItemView, IDerivedItemView>);


	class DerivedItem : public Item
	{
	public:

		void SetString(std::string_view stringValue)
		{
			m_String = stringValue;
		}

		void DoDerivedItemStuff()
		{
			vsb::SafePtr<IDerivedItemView> derivedView = GetView<IDerivedItemView>();

			if (auto* pView = derivedView.ValidateAndGet())
			{
				pView->DoDerivedItemStuff(m_String);
			}
		}

	private:

		std::string m_String {};
	};


	class ItemView : public vsb::Object, public virtual IItemView
	{
	public:

		ItemView() : vsb::Object(vsb::ObjectHint::Scoped)
		{}

		void DoBaseItemStuff(int x, int y) override
		{
			VSBLOG_INFO("DoBaseItemStuff: {}, {}", x, y);
		}
	};


	class DerivedItemView : public ItemView, public IDerivedItemView
	{
	public:

		void DoDerivedItemStuff(std::string_view stringValue) override
		{
			VSBLOG_INFO("DoDerivedItemStuff: {}", stringValue);
		}

	private:

		int m_Value {};
	};


	class ClassA
	{
	public:
		virtual ~ClassA() = default;
	private:

		int m_Value {};


	};


	class ClassB
	{
	public:
		virtual ~ClassB() = default; //commenting this out will invalidate vsb::SafeCast()
	private:

		float m_Value {};


	};



}


int main()
{
	VSBLOG_INFO("Multiple inheritance testing");

	auto viewSize = sizeof(DerivedItemView);
	VSBLOG_INFO("Size of DerivedItemView: {} bytes", viewSize);


	DerivedItem item;
	DerivedItemView itemView;

	item.SetString("Hello!");
	item.SetPosition(123, 456);

	item.RegisterView(&itemView);

	item.DoBaseItemStuff();
	item.DoDerivedItemStuff();

	item.UnregisterView();
	item.DoDerivedItemStuff();
	item.DoBaseItemStuff();


	ClassA a;
	ClassB b;

	ClassA* pA = vsb::SafeCast<ClassA>(&b);

	VSBLOG_INFO("pA = {}", reinterpret_cast<intptr_t>(pA));

}