//
// Created by Alexander on 23.01.2026.
//


#include "vsb.h"
#include "objects/destruction_central.h"
#include "vsb/objects/internal/object_registry.h"

namespace vsb
{
	void VSBInit()
	{
		internal::ObjectRegistry::Init();
		DestructionCentral::InitDefault();
	}
}