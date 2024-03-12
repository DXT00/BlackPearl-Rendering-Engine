#pragma once
#include "RefCountPtr.h"
namespace BlackPearl {
	/** An enumeration of the different RHI reference types. */
	enum RHIResourceType : uint8_t
	{
        RT_None,
        RT_Texture_SRV,            // vk - SampledImage VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
        RT_Texture_UAV,            // vk - StorageImage VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
        RT_Texture_InputAttachment, // vk - VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
        RT_TypedBuffer_SRV,        // vk - UniformTexelBuffer VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
        RT_TypedBuffer_UAV,        // vk - StorageTexelBuffer VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
        RT_StructuredBuffer_SRV,   // vk - StorageBuffer VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
        RT_StructuredBuffer_UAV,   // vk - StorageBuffer VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
        RT_RawBuffer_SRV,          // vk - StorageBuffer VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
        RT_RawBuffer_UAV,          // vk - StorageBuffer VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
        RT_ConstantBuffer,         // vk - Uniform buffer VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
        RT_VolatileConstantBuffer, // vk - UniformBufferDynamic VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC

        RT_Sampler,                // vk - Sampler VK_DESCRIPTOR_TYPE_SAMPLER
        RT_RayTracingAccelStruct,   // vk -AccelerationStructureKHR VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR
        RT_PushConstants,

        RT_Count
	};


    class IResource
    {
    protected:
        IResource() = default;
        virtual ~IResource() = default;

    public:
        virtual unsigned long AddRef() = 0;
        virtual unsigned long Release() = 0;

        // Returns a native object or interface, for example ID3D11Device*, or nullptr if the requested interface is unavailable.
        // Does *not* AddRef the returned interface.
        //virtual Object getNativeObject(ObjectType objectType) { (void)objectType; return nullptr; }

        // Non-copyable and non-movable
        IResource(const IResource&) = delete;
        IResource(const IResource&&) = delete;
        IResource& operator=(const IResource&) = delete;
        IResource& operator=(const IResource&&) = delete;
    };
    typedef RefCountPtr<IResource> ResourceHandle;


}

