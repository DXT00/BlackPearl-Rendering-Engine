#pragma once
#include "RHIDefinitions.h"
#include "RefCountPtr.h"
#include "RHIResources.h"
namespace BlackPearl {
    struct BufferDesc
    {
        uint64_t byteSize = 0;
        uint32_t structStride = 0; // if non-zero it's structured
        uint32_t maxVersions = 0; // only valid and required to be nonzero for volatile buffers on Vulkan
        std::string debugName;
        Format format = Format::UNKNOWN; // for typed buffer views
        bool canHaveUAVs = false;
        bool canHaveTypedViews = false;
        bool canHaveRawViews = false;
        bool isVertexBuffer = false;
        bool isIndexBuffer = false;
        bool isConstantBuffer = false;
        bool isDrawIndirectArgs = false;
        bool isAccelStructBuildInput = false;
        bool isAccelStructStorage = false;
        bool isShaderBindingTable = false;

        // A dynamic/upload buffer whose contents only live in the current command list
        bool isVolatile = false;

        // Indicates that the buffer is created with no backing memory,
        // and memory is bound to the texture later using bindBufferMemory.
        // On DX12, the buffer resource is created at the time of memory binding.
        bool isVirtual = false;

        ResourceStates initialState = ResourceStates::Common;

        // see TextureDesc::keepInitialState
        bool keepInitialState = false;

        CpuAccessMode cpuAccess = CpuAccessMode::None;

        SharedResourceFlags sharedResourceFlags = SharedResourceFlags::None;

         BufferDesc& setByteSize(uint64_t value) { byteSize = value; return *this; }
         BufferDesc& setStructStride(uint32_t value) { structStride = value; return *this; }
         BufferDesc& setMaxVersions(uint32_t value) { maxVersions = value; return *this; }
        BufferDesc& setDebugName(const std::string& value) { debugName = value; return *this; }
         BufferDesc& setFormat(Format value) { format = value; return *this; }
         BufferDesc& setCanHaveUAVs(bool value) { canHaveUAVs = value; return *this; }
         BufferDesc& setCanHaveTypedViews(bool value) { canHaveTypedViews = value; return *this; }
         BufferDesc& setCanHaveRawViews(bool value) { canHaveRawViews = value; return *this; }
         BufferDesc& setIsVertexBuffer(bool value) { isVertexBuffer = value; return *this; }
         BufferDesc& setIsIndexBuffer(bool value) { isIndexBuffer = value; return *this; }
         BufferDesc& setIsConstantBuffer(bool value) { isConstantBuffer = value; return *this; }
         BufferDesc& setIsDrawIndirectArgs(bool value) { isDrawIndirectArgs = value; return *this; }
         BufferDesc& setIsAccelStructBuildInput(bool value) { isAccelStructBuildInput = value; return *this; }
         BufferDesc& setIsAccelStructStorage(bool value) { isAccelStructStorage = value; return *this; }
         BufferDesc& setIsShaderBindingTable(bool value) { isShaderBindingTable = value; return *this; }
         BufferDesc& setIsVolatile(bool value) { isVolatile = value; return *this; }
         BufferDesc& setIsVirtual(bool value) { isVirtual = value; return *this; }
         BufferDesc& setInitialState(ResourceStates value) { initialState = value; return *this; }
         BufferDesc& setKeepInitialState(bool value) { keepInitialState = value; return *this; }
         BufferDesc& setCpuAccess(CpuAccessMode value) { cpuAccess = value; return *this; }
    };

    struct BufferRange
    {
        uint64_t byteOffset = 0;
        uint64_t byteSize = 0;

        BufferRange() = default;

        BufferRange(uint64_t _byteOffset, uint64_t _byteSize)
            : byteOffset(_byteOffset)
            , byteSize(_byteSize)
        { }

        [[nodiscard]] BufferRange resolve(const BufferDesc& desc) const;
        [[nodiscard]]  bool isEntireBuffer(const BufferDesc& desc) const { return (byteOffset == 0) && (byteSize == ~0ull || byteSize == desc.byteSize); }
         bool operator== (const BufferRange& other) const { return byteOffset == other.byteOffset && byteSize == other.byteSize; }

         BufferRange& setByteOffset(uint64_t value) { byteOffset = value; return *this; }
         BufferRange& setByteSize(uint64_t value) { byteSize = value; return *this; }
    };

    static const BufferRange EntireBuffer = BufferRange(0, ~0ull);

    class IBuffer :public IResource
    {
    public:
        [[nodiscard]] virtual const BufferDesc& getDesc() const = 0;
    };

    typedef RefCountPtr<IBuffer> BufferHandle;

}

