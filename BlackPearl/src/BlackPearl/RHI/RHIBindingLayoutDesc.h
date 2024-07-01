/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
#pragma once
#include "RHIResources.h"
#include "RHIDefinitions.h"

namespace BlackPearl {

    struct RHIBindingLayoutItem
    {
        uint32_t slot;

        RHIResourceType type : 8;
        uint8_t unused : 8;
        uint16_t size : 16;

        bool operator ==(const RHIBindingLayoutItem& b) const {
            return slot == b.slot
                && type == b.type
                && size == b.size;
        }
        bool operator !=(const RHIBindingLayoutItem& b) const { return !(*this == b); }


        // Helper functions for strongly typed initialization
#define NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(TYPE) /* NOLINT(cppcoreguidelines-macro-usage) */ \
        static RHIBindingLayoutItem TYPE(const uint32_t slot) { \
            RHIBindingLayoutItem result{}; \
            result.slot = slot; \
            result.type = RHIResourceType::TYPE; \
            return result; }

        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_Texture_SRV)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_Texture_UAV)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_TypedBuffer_SRV)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_TypedBuffer_UAV)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_StructuredBuffer_SRV)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_StructuredBuffer_UAV)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_RawBuffer_SRV)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_RawBuffer_UAV)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_ConstantBuffer)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_VolatileConstantBuffer)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_Sampler)
        NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER(RT_RayTracingAccelStruct)

            static RHIBindingLayoutItem RT_PushConstants(const uint32_t slot, const size_t size)
        {
            RHIBindingLayoutItem result{};
            result.slot = slot;
            result.type = RHIResourceType::RT_PushConstants;
            result.size = uint16_t(size);
            return result;
        }
#undef NVRHI_BINDING_LAYOUT_ITEM_INITIALIZER
    };
    // verify the packing of BindingLayoutItem for good alignment
    static_assert(sizeof(RHIBindingLayoutItem) == 8, "sizeof(BindingLayoutItem) is supposed to be 8 bytes");

    typedef nvrhi::static_vector<RHIBindingLayoutItem, c_MaxBindingsPerLayout> BindingLayoutItemArray;


    struct VulkanBindingOffsets
    {
        uint32_t shaderResource = 0;
        uint32_t sampler = 128;
        uint32_t constantBuffer = 256;
        uint32_t unorderedAccess = 384;

        constexpr VulkanBindingOffsets& setShaderResourceOffset(uint32_t value) { shaderResource = value; return *this; }
        constexpr VulkanBindingOffsets& setSamplerOffset(uint32_t value) { sampler = value; return *this; }
        constexpr VulkanBindingOffsets& setConstantBufferOffset(uint32_t value) { constantBuffer = value; return *this; }
        constexpr VulkanBindingOffsets& setUnorderedAccessViewOffset(uint32_t value) { unorderedAccess = value; return *this; }
    };

	class RHIBindingLayoutDesc
	{
    public:
        ShaderType visibility = ShaderType::None;
        uint32_t registerSpace = 0;

        // This flag controls the validation behavior for pipelines that use multiple binding layouts.
        // - When it's set to `false`, the `registerSpace` parameter only affects the DX12 implementation,
        //   and the validation layer will report an error when non-zero `registerSpace` is used with other APIs.
        // - When it's set to `true`, the `registerSpace` parameter is assumed to be the same as the descriptor set
        //   index on Vulkan. Since binding layouts and binding sets map to Vulkan descriptor sets 1:1,
        //   that means if a pipeline is using multiple binding layouts, layout 0 must have `registerSpace = 0`,
        //   layout 1 must have `registerSpace = 1` and so on. NVRHI validation layer will verify that and
        //   report errors on pipeline creation when register spaces don't match layout indices.
        //   The motivation for such validation is that DXC maps register spaces to Vulkan descriptor sets by default.
        bool registerSpaceIsDescriptorSet = false;

        BindingLayoutItemArray bindings;
        VulkanBindingOffsets bindingOffsets;

        RHIBindingLayoutDesc& setVisibility(ShaderType value) { visibility = value; return *this; }
        RHIBindingLayoutDesc& setRegisterSpace(uint32_t value) { registerSpace = value; return *this; }
        RHIBindingLayoutDesc& setRegisterSpaceIsDescriptorSet(bool value) { registerSpaceIsDescriptorSet = value; return *this; }
        RHIBindingLayoutDesc& addItem(const RHIBindingLayoutItem& value) { bindings.push_back(value); return *this; }
        RHIBindingLayoutDesc& setBindingOffsets(const VulkanBindingOffsets& value) { bindingOffsets = value; return *this; }
	};

    // Bindless layouts allow applications to attach a descriptor table to an unbounded
   // resource array in the shader. The size of the array is not known ahead of time.
   // The same table can be bound to multiple register spaces on DX12, in order to 
   // access different types of resources stored in the table through different arrays.
   // The `registerSpaces` vector specifies which spaces will the table be bound to,
   // with the table type (SRV or UAV) derived from the resource type assigned to each space.
    struct RHIBindlessLayoutDesc
    {
        ShaderType visibility = ShaderType::None;
        uint32_t firstSlot = 0;
        uint32_t maxCapacity = 0;
        std::vector<RHIBindingLayoutItem> registerSpaces;

        RHIBindlessLayoutDesc& setVisibility(ShaderType value) { visibility = value; return *this; }
        RHIBindlessLayoutDesc& setFirstSlot(uint32_t value) { firstSlot = value; return *this; }
        RHIBindlessLayoutDesc& setMaxCapacity(uint32_t value) { maxCapacity = value; return *this; }
        RHIBindlessLayoutDesc& addRegisterSpace(const RHIBindingLayoutItem& value) { registerSpaces.push_back(value); return *this; }
    };

    class IBindingLayout : public IResource
    {
    public:
        [[nodiscard]] virtual const RHIBindingLayoutDesc* getDesc() const = 0;           // returns nullptr for bindless layouts
        [[nodiscard]] virtual const RHIBindlessLayoutDesc* getBindlessDesc() const = 0;  // returns nullptr for regular layouts
    };
    typedef RefCountPtr<IBindingLayout> BindingLayoutHandle;

}
