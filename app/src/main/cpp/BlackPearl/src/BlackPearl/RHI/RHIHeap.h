#pragma once
#include "RHIResources.h"
namespace BlackPearl {
    enum class HeapType : uint8_t
    {
        DeviceLocal,
        Upload,
        Readback
    };
    struct HeapDesc
    {
        uint64_t capacity = 0;
        HeapType type;
        std::string debugName;

        const HeapDesc& setCapacity(uint64_t value) { capacity = value; return *this; }
        const HeapDesc& setType(HeapType value) { type = value; return *this; }
        HeapDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
    };
    class IHeap : public IResource
    {
    public:
        virtual const HeapDesc& getDesc() = 0;
    };

    typedef RefCountPtr<IHeap> HeapHandle;

}

