#pragma once
#include "RHIBindingSet.h"
// Descriptor tables are bare, without extra mappings, state, or liveness tracking.
// Unlike binding sets, descriptor tables are mutable - moreover, modification is the only way to populate them.
// They can be grown or shrunk, and they are not tied to any binding layout.
// All tracking is off, so applications should use descriptor tables with great care.
// IDescriptorTable is derived from IBindingSet to allow mixing them in the binding arrays.
namespace BlackPearl {
	class IDescriptorTable : public IBindingSet
	{
	public:
		[[nodiscard]] virtual uint32_t getCapacity() const = 0;
	};

	typedef RefCountPtr<IDescriptorTable> DescriptorTableHandle;
}
