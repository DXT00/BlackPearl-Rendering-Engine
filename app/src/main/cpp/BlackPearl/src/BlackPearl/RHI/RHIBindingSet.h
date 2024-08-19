#pragma once
#include<stdint.h>
#include "RHIDefinitions.h"
#include "RHIResources.h"
#include "RHITexture.h"
#include "RHIBuffer.h"
#include "RHISampler.h"
#include "RHIBindingLayoutDesc.h"
#include "RHIAccelStruct.h"
namespace BlackPearl {
    struct BindingSetItem
    {
        IResource* resourceHandle;

        uint32_t slot;

        RHIResourceType type : 8;
        TextureDimension dimension : 8; // valid for Texture_SRV, Texture_UAV
        Format format : 8; // valid for Texture_SRV, Texture_UAV, Buffer_SRV, Buffer_UAV
        uint8_t unused : 8;

        union
        {
            TextureSubresourceSet subresources; // valid for Texture_SRV, Texture_UAV
            BufferRange range; // valid for Buffer_SRV, Buffer_UAV, ConstantBuffer
            uint64_t rawData[2];
        };

        // verify that the `subresources` and `range` have the same size and are covered by `rawData`
        static_assert(sizeof(TextureSubresourceSet) == 16, "sizeof(TextureSubresourceSet) is supposed to be 16 bytes");
        static_assert(sizeof(BufferRange) == 16, "sizeof(BufferRange) is supposed to be 16 bytes");

        bool operator ==(const BindingSetItem& b) const
        {
            return resourceHandle == b.resourceHandle
                && slot == b.slot
                && type == b.type
                && dimension == b.dimension
                && format == b.format
                && rawData[0] == b.rawData[0]
                && rawData[1] == b.rawData[1];
        }

        bool operator !=(const BindingSetItem& b) const
        {
            return !(*this == b);
        }

        // Default constructor that doesn't initialize anything for performance:
        // BindingSetItem's are stored in large statically sized arrays.
        BindingSetItem() { }  // NOLINT(cppcoreguidelines-pro-type-member-init, modernize-use-equals-default)

        // Helper functions for strongly typed initialization

        static BindingSetItem None(uint32_t slot = 0)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_None;
            result.resourceHandle = nullptr;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.rawData[0] = 0;
            result.rawData[1] = 0;
            result.unused = 0;
            return result;
        }

        static TextureSubresourceSet AllSubresources ;

        static BindingSetItem Texture_SRV(uint32_t slot, ITexture* texture, Format format = Format::UNKNOWN,
            TextureSubresourceSet subresources = AllSubresources, TextureDimension dimension = TextureDimension::Unknown)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_Texture_SRV;
            result.resourceHandle = texture;
            result.format = format;
            result.dimension = dimension;
            result.subresources = subresources;
            result.unused = 0;
            return result;
        }

        static BindingSetItem Texture_UAV(uint32_t slot, ITexture* texture, Format format = Format::UNKNOWN,
            TextureSubresourceSet subresources = TextureSubresourceSet(0, 1, 0, TextureSubresourceSet::AllArraySlices),
            TextureDimension dimension = TextureDimension::Unknown)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_Texture_UAV;
            result.resourceHandle = texture;
            result.format = format;
            result.dimension = dimension;
            result.subresources = subresources;
            result.unused = 0;
            return result;
        }

        static BindingSetItem TypedBuffer_SRV(uint32_t slot, IBuffer* buffer, Format format = Format::UNKNOWN, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_TypedBuffer_SRV;
            result.resourceHandle = buffer;
            result.format = format;
            result.dimension = TextureDimension::Unknown;
            result.range = range;
            result.unused = 0;
            return result;
        }

        static BindingSetItem TypedBuffer_UAV(uint32_t slot, IBuffer* buffer, Format format = Format::UNKNOWN, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_TypedBuffer_UAV;
            result.resourceHandle = buffer;
            result.format = format;
            result.dimension = TextureDimension::Unknown;
            result.range = range;
            result.unused = 0;
            return result;
        }

        static BindingSetItem ConstantBuffer(uint32_t slot, IBuffer* buffer, BufferRange range = EntireBuffer)
        {
            bool isVolatile = buffer && buffer->getDesc().isVolatile;

            BindingSetItem result;
            result.slot = slot;
            result.type = isVolatile ? RHIResourceType::RT_VolatileConstantBuffer : RHIResourceType::RT_ConstantBuffer;
            result.resourceHandle = buffer;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.range = range;
            result.unused = 0;
            return result;
        }

        static BindingSetItem Sampler(uint32_t slot, ISampler* sampler)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_Sampler;
            result.resourceHandle = sampler;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.rawData[0] = 0;
            result.rawData[1] = 0;
            result.unused = 0;
            return result;
        }

        static BindingSetItem RayTracingAccelStruct(uint32_t slot, IAccelStruct* as)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_RayTracingAccelStruct;
            result.resourceHandle = as;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.rawData[0] = 0;
            result.rawData[1] = 0;
            result.unused = 0;
            return result;
        }

        static BindingSetItem StructuredBuffer_SRV(uint32_t slot, IBuffer* buffer, Format format = Format::UNKNOWN, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_StructuredBuffer_SRV;
            result.resourceHandle = buffer;
            result.format = format;
            result.dimension = TextureDimension::Unknown;
            result.range = range;
            result.unused = 0;
            return result;
        }

        static BindingSetItem StructuredBuffer_UAV(uint32_t slot, IBuffer* buffer, Format format = Format::UNKNOWN, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_StructuredBuffer_UAV;
            result.resourceHandle = buffer;
            result.format = format;
            result.dimension = TextureDimension::Unknown;
            result.range = range;
            result.unused = 0;
            return result;
        }

        static BindingSetItem RawBuffer_SRV(uint32_t slot, IBuffer* buffer, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_RawBuffer_SRV;
            result.resourceHandle = buffer;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.range = range;
            result.unused = 0;
            return result;
        }

        static BindingSetItem RawBuffer_UAV(uint32_t slot, IBuffer* buffer, BufferRange range = EntireBuffer)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_RawBuffer_UAV;
            result.resourceHandle = buffer;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.range = range;
            result.unused = 0;
            return result;
        }

        static BindingSetItem PushConstants(uint32_t slot, uint32_t byteSize)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_PushConstants;
            result.resourceHandle = nullptr;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Unknown;
            result.range.byteOffset = 0;
            result.range.byteSize = byteSize;
            result.unused = 0;
            return result;
        }
        static BindingSetItem InputAttachment(uint32_t slot, ITexture* texture)
        {
            BindingSetItem result;
            result.slot = slot;
            result.type = RHIResourceType::RT_Texture_InputAttachment;
            result.resourceHandle = texture;
            result.format = Format::UNKNOWN;
            result.dimension = TextureDimension::Texture2D;
            result.unused = 0;
            return result;
        }

        BindingSetItem& setFormat(Format value) { format = value; return *this; }
        BindingSetItem& setDimension(TextureDimension value) { dimension = value; return *this; }
        BindingSetItem& setSubresources(TextureSubresourceSet value) { subresources = value; return *this; }
        BindingSetItem& setRange(BufferRange value) { range = value; return *this; }
    };

    // verify the packing of BindingSetItem for good alignment
    //static_assert(sizeof(BindingSetItem) == 32, "sizeof(BindingSetItem) is supposed to be 32 bytes");

    // describes the resource bindings for a single pipeline stage

    // describes a set of bindings across all stages of the pipeline
    // (not all bindings need to be present in the set, but the set must be defined by a single BindingSetItem object)
    struct BindingSetDesc
    {
         std::vector<BindingSetItem> bindings;

        // Enables automatic liveness tracking of this binding set by nvrhi command lists.
        // By setting trackLiveness to false, you take the responsibility of not releasing it 
        // until all rendering commands using the binding set are finished.
        bool trackLiveness = true;

        bool operator ==(const BindingSetDesc& b) const
        {
            if (bindings.size() != b.bindings.size())
                return false;

            for (size_t i = 0; i < bindings.size(); ++i)
            {
                if (bindings[i] != b.bindings[i])
                    return false;
            }

            return true;
        }

        bool operator !=(const BindingSetDesc& b) const
        {
            return !(*this == b);
        }

        BindingSetDesc& addItem(const BindingSetItem& value) { bindings.push_back(value); return *this; }
        BindingSetDesc& setTrackLiveness(bool value) { trackLiveness = value; return *this; }
    };

    class IBindingSet :public IResource
    {
    public:
        virtual const BindingSetDesc* getDesc() const = 0;  // returns nullptr for descriptor tables
        virtual IBindingLayout* getLayout() const = 0;
    };

    typedef RefCountPtr<IBindingSet> BindingSetHandle;



    struct VertexBufferBinding
    {
        IBuffer* buffer = nullptr;
        uint32_t slot;
        uint64_t offset;

        bool operator ==(const VertexBufferBinding& b) const
        {
            return buffer == b.buffer
                && slot == b.slot
                && offset == b.offset;
        }
        bool operator !=(const VertexBufferBinding& b) const { return !(*this == b); }

        VertexBufferBinding& setBuffer(IBuffer* value) { buffer = value; return *this; }
        VertexBufferBinding& setSlot(uint32_t value) { slot = value; return *this; }
        VertexBufferBinding& setOffset(uint64_t value) { offset = value; return *this; }
    };

    struct IndexBufferBinding
    {
        IBuffer* buffer = nullptr;
        Format format;
        uint32_t offset;

        bool operator ==(const IndexBufferBinding& b) const
        {
            return buffer == b.buffer
                && format == b.format
                && offset == b.offset;
        }
        bool operator !=(const IndexBufferBinding& b) const { return !(*this == b); }

        IndexBufferBinding& setBuffer(IBuffer* value) { buffer = value; return *this; }
        IndexBufferBinding& setFormat(Format value) { format = value; return *this; }
        IndexBufferBinding& setOffset(uint32_t value) { offset = value; return *this; }
    };

    typedef std::vector<IBindingSet*> BindingSetVector;
}
