#pragma once
#include "BlackPearl/RHI/RHIRayTraceStruct.h"
#include "BlackPearl/RHI/VulkanRHI/VkContext.h"
namespace BlackPearl {
    //static VkDeviceOrHostAddressConstKHR getBufferAddress(IBuffer* _buffer, uint64_t offset);
    class AccelStruct : public RefCounter<rt::IAccelStruct>
    {
    public:
        BufferHandle dataBuffer;
        std::vector<VkAccelerationStructureInstanceKHR> instances;
        VkAccelerationStructureKHR accelStruct;
        VkDeviceAddress accelStructDeviceAddress = 0;
        rt::AccelStructDesc desc;
        bool allowUpdate = false;
        bool compacted = false;
        size_t rtxmuId = ~0ull;
        VkBuffer rtxmuBuffer;


        explicit AccelStruct(const VulkanContext& context)
            : m_Context(context)
        { }

        ~AccelStruct() override;

        const rt::AccelStructDesc& getDesc() const override { return desc; }
        bool isCompacted() const override { return compacted; }
        uint64_t getDeviceAddress() const override;

    private:
        const VulkanContext& m_Context;
    };

	class OpacityMicromap : public RefCounter<rt::IOpacityMicromap>
	{

    public:
        BufferHandle dataBuffer;
        VkMicromapEXT opacityMicromap;
        rt::OpacityMicromapDesc desc;
        bool allowUpdate = false;
        bool compacted = false;

        explicit OpacityMicromap(const VulkanContext& context)
            : m_Context(context)
        { }

        ~OpacityMicromap() override;

        //Object getNativeObject(ObjectType objectType) override;
        const rt::OpacityMicromapDesc& getDesc() const override { return desc; }
        bool isCompacted() const override { return compacted; }
        uint64_t getDeviceAddress() const override;

    private:
        const VulkanContext& m_Context;
	};
}

