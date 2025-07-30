//(C) 2025 Alexander Samarin

#include "container_defs.h"

#include "vsb/debug.h"


namespace vsb
{
	FromEnd::FromEnd(const Index theOffset, const bool strict): offset(theOffset)
	{
		if (strict && offset < 1)
		{
			VSBLOG_WARN("Offset should be >= 1. Actual: {}", offset);
			VSB_BREAK
		}
	}


	Index FromEnd::GetIndex(const Count size) const noexcept
	{
		return size - offset;
	}

}


