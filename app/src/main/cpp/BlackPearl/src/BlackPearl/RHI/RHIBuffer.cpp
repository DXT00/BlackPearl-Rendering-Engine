#include "pch.h"
#include "RHIBuffer.h"

namespace  BlackPearl {
    BufferRange BufferRange::resolve(const BufferDesc& desc) const
    {
        BufferRange result;
        result.byteOffset = std::min(byteOffset, desc.byteSize);
        if (byteSize == 0)
            result.byteSize = desc.byteSize - result.byteOffset;
        else
            result.byteSize = std::min(byteSize, desc.byteSize - result.byteOffset);
        return result;
    }

}
