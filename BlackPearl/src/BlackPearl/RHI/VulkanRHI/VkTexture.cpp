#include "pch.h"
#include "VkTexture.h"
#include "VkEnum.h"
#include "VkUtil.h"
#include "VkDevice.h"
#include "BlackPearl/RHI/Common/FormatInfo.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/Core.h"
#include <mutex>
#include <utility>

namespace BlackPearl {
    static VkImageViewType textureDimensionToImageViewType(TextureDimension dimension)
    {
        switch (dimension)
        {
        case TextureDimension::Texture1D:
            return VK_IMAGE_VIEW_TYPE_1D;

        case TextureDimension::Texture1DArray:
            return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

        case TextureDimension::Texture2D:
        case TextureDimension::Texture2DMS:
            return VK_IMAGE_VIEW_TYPE_2D;

        case TextureDimension::Texture2DArray:
        case TextureDimension::Texture2DMSArray:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

        case TextureDimension::TextureCube:
            return VK_IMAGE_VIEW_TYPE_CUBE;

        case TextureDimension::TextureCubeArray:
            return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

        case TextureDimension::Texture3D:
            return VK_IMAGE_VIEW_TYPE_3D;

        case TextureDimension::Unknown:
        default:
            GE_INVALID_ENUM();
            return VK_IMAGE_VIEW_TYPE_2D;
        }
    }
    // infer aspect flags for a given image format
    VkImageAspectFlags guessImageAspectFlags(VkFormat format)
    {
        switch (format)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
            return VK_IMAGE_ASPECT_DEPTH_BIT;

        case VK_FORMAT_S8_UINT:
            return VK_IMAGE_ASPECT_STENCIL_BIT;

        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }
    VkImageAspectFlags guessSubresourceImageAspectFlags(VkFormat format, ETexture::TextureSubresourceViewType viewType)
    {
        VkImageAspectFlags flags = guessImageAspectFlags(format);
        if ((flags & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
            == (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
        {
            if (viewType == ETexture::TextureSubresourceViewType::DepthOnly)
            {
                flags = flags & (~VK_IMAGE_ASPECT_STENCIL_BIT);
            }
            else if (viewType == ETexture::TextureSubresourceViewType::StencilOnly)
            {
                flags = flags & (~VK_IMAGE_ASPECT_DEPTH_BIT);
            }
        }
        return flags;
    }

    static VkImageType textureDimensionToImageType(TextureDimension dimension)
    {
        switch (dimension)
        {
        case TextureDimension::Texture1D:
        case TextureDimension::Texture1DArray:
            return VK_IMAGE_TYPE_1D;

        case TextureDimension::Texture2D:
        case TextureDimension::Texture2DArray:
        case TextureDimension::TextureCube:
        case TextureDimension::TextureCubeArray:
        case TextureDimension::Texture2DMS:
        case TextureDimension::Texture2DMSArray:
            return VK_IMAGE_TYPE_2D;

        case TextureDimension::Texture3D:
            return VK_IMAGE_TYPE_3D;

        case TextureDimension::Unknown:
        default:
            GE_INVALID_ENUM();
            return VK_IMAGE_TYPE_2D;
        }
    }


    /*static VkImageViewType textureDimensionToImageViewType(TextureDimension dimension)
    {
        switch (dimension)
        {
        case TextureDimension::Texture1D:
            return VK_IMAGE_VIEW_TYPE_1D;

        case TextureDimension::Texture1DArray:
            return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

        case TextureDimension::Texture2D:
        case TextureDimension::Texture2DMS:
            return VK_IMAGE_VIEW_TYPE_2D;

        case TextureDimension::Texture2DArray:
        case TextureDimension::Texture2DMSArray:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

        case TextureDimension::TextureCube:
            return VK_IMAGE_VIEW_TYPE_CUBE;

        case TextureDimension::TextureCubeArray:
            return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

        case TextureDimension::Texture3D:
            return VK_IMAGE_VIEW_TYPE_3D;

        case TextureDimension::Unknown:
        default:
            GE_INVALID_ENUM();
            return VK_IMAGE_VIEW_TYPE_2D;
        }
    }*/

    static VkExtent3D pickImageExtent(const TextureDesc& d)
    {
        VkExtent3D extend{};
        extend.width = d.width;
        extend.height = d.height;
        extend.depth = d.depth;
        return extend;
    }
    
    static VkImageUsageFlags pickImageUsage(const TextureDesc& d)
    {
        const FormatInfo& formatInfo = getFormatInfo(d.format);

        VkImageUsageFlags ret = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        if (d.isShaderResource)
            ret |= VK_IMAGE_USAGE_SAMPLED_BIT;

        if (d.isRenderTarget)
        {
            if (formatInfo.hasDepth || formatInfo.hasStencil)
            {
                ret |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            }
            else {
                ret |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            }
        }

        if (d.isUAV)
            ret |= VK_IMAGE_USAGE_STORAGE_BIT;

        if (d.isShadingRateSurface)
            ret |= VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

        return ret;
    }

    static VkSampleCountFlagBits pickImageSampleCount(const TextureDesc& d)
    {
        switch (d.sampleCount)
        {
        case 1:
            return VK_SAMPLE_COUNT_1_BIT;

        case 2:
            return VK_SAMPLE_COUNT_2_BIT;

        case 4:
            return VK_SAMPLE_COUNT_4_BIT;

        case 8:
            return VK_SAMPLE_COUNT_8_BIT;

        case 16:
            return VK_SAMPLE_COUNT_16_BIT;

        case 32:
            return VK_SAMPLE_COUNT_32_BIT;

        case 64:
            return VK_SAMPLE_COUNT_64_BIT;

        default:
            GE_INVALID_ENUM();
            return VK_SAMPLE_COUNT_1_BIT;
        }
    }
    VkImageCreateFlags pickImageFlags(const TextureDesc& d)
    {
        VkImageCreateFlags flags = VkImageCreateFlags(0);

        if (d.dimension == TextureDimension::TextureCube ||
            d.dimension == TextureDimension::TextureCubeArray)
            flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

        if (d.isTypeless)
            flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

        return flags;
    }
    // fills out all info fields in Texture based on a TextureDesc
    static void fillTextureInfo(ETexture* texture, const TextureDesc& desc)
    {
        texture->desc = desc;

        VkImageType type = textureDimensionToImageType(desc.dimension);
        VkExtent3D extent = pickImageExtent(desc);
        uint32_t numLayers = desc.arraySize;
        VkFormat format = VkUtil::convertFormat(desc.format);
        VkImageUsageFlags usage = pickImageUsage(desc);
        VkSampleCountFlagBits sampleCount = pickImageSampleCount(desc);
        VkImageCreateFlags flags = pickImageFlags(desc);

      /*  texture->imageInfo = VkImageCreateInfo()
            .setImageType(type)
            .setExtent(extent)
            .setMipLevels(desc.mipLevels)
            .setArrayLayers(numLayers)
            .setFormat(format)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSamples(sampleCount)
            .setFlags(flags);*/
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent = extent;
        
        imageInfo.mipLevels = desc.mipLevels;
        imageInfo.arrayLayers = numLayers;
        imageInfo.format = format;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = sampleCount;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = flags;

        texture->imageInfo = imageInfo;
#if _WIN32
        const auto handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#else
        const auto handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
#endif

        VkExternalMemoryImageCreateInfo exteralMemInfo{};
        exteralMemInfo.handleTypes = handleType;
        /*texture->externalMemoryImageInfo = vk::ExternalMemoryImageCreateInfo()
            .setHandleTypes(handleType);*/

        if (desc.sharedResourceFlags == SharedResourceFlags::Shared)
            texture->imageInfo.pNext = (&texture->externalMemoryImageInfo);
    }



    TextureSubresourceView& ETexture::getSubresourceView(const TextureSubresourceSet& subresource, TextureDimension dimension, Format format, TextureSubresourceViewType viewtype)
	{
        // This function is called from createBindingSet etc. and therefore free-threaded.
       // It modifies the subresourceViews map associated with the texture.
        std::lock_guard lockGuard(m_Mutex);

        if (dimension == TextureDimension::Unknown)
            dimension = desc.dimension;

        if (format == Format::UNKNOWN)
            format = desc.format;

        auto cachekey = std::make_tuple(subresource, viewtype, dimension, format);
        auto iter = subresourceViews.find(cachekey);
        if (iter != subresourceViews.end())
        {
            return iter->second;
        }

        auto iter_pair = subresourceViews.emplace(cachekey, *this);
        auto& view = std::get<0>(iter_pair)->second;

        view.subresource = subresource;

        auto vkformat = VkUtil::convertFormat(format);

        VkImageAspectFlags aspectflags = guessSubresourceImageAspectFlags(VkFormat(vkformat), viewtype);
        VkImageSubresourceRange srRange;

        srRange.aspectMask = aspectflags;
        srRange.baseMipLevel = subresource.baseMipLevel;
        srRange.levelCount = subresource.numMipLevels;
        srRange.baseArrayLayer = subresource.baseArraySlice;
        srRange.layerCount = subresource.numArraySlices;

        view.subresourceRange = srRange;
           /* .setAspectMask(aspectflags)
            .setBaseMipLevel(subresource.baseMipLevel)
            .setLevelCount(subresource.numMipLevels)
            .setBaseArrayLayer(subresource.baseArraySlice)
            .setLayerCount(subresource.numArraySlices);*/

        VkImageViewType imageViewType = textureDimensionToImageViewType(dimension);

        VkImageViewCreateInfo viewInfo;
        viewInfo.image = image;
        viewInfo.viewType = imageViewType;
        viewInfo.format = vkformat;
        viewInfo.subresourceRange = view.subresourceRange;
            /*.setImage(image)
            .setViewType(imageViewType)
            .setFormat(vk::Format(vkformat))
            .setSubresourceRange(view.subresourceRange);*/

        if (viewtype == TextureSubresourceViewType::StencilOnly)
        {
            // D3D / HLSL puts stencil values in the second component to keep the illusion of combined depth/stencil.
            // Set a component swizzle so we appear to do the same.
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        }
        const VkResult res = vkCreateImageView(m_Context.device, &viewInfo, m_Context.allocationCallbacks, &view.view);
        GE_ASSERT(res == VkResult::VK_SUCCESS, "CreateImageView fail");

        const std::string debugName = std::string("ImageView for: ") + (desc.debugName);
        m_Context.nameVKObject(VkImageView(view.view), VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT, debugName.c_str());

        return view;
	}
	uint32_t ETexture::getNumSubresources() const
	{
        return desc.mipLevels * desc.arraySize;
	}
	uint32_t ETexture::getSubresourceIndex(uint32_t mipLevel, uint32_t arrayLayer) const
	{
        return mipLevel * desc.arraySize + arrayLayer;
	}
	ETexture::~ETexture()
	{
        for (auto& viewIter : subresourceViews)
        {
            auto& view = viewIter.second.view;
            vkDestroyImageView(m_Context.device, view, m_Context.allocationCallbacks);
            //m_Context.device.destroyImageView(view, m_Context.allocationCallbacks);
            view = VkImageView();
        }
        subresourceViews.clear();

        if (managed)
        {
            if (image)
            {
                vkDestroyImage(m_Context.device, image, m_Context.allocationCallbacks);
               // m_Context.device.destroyImage(image, m_Context.allocationCallbacks);
                //image = VkImage();
            }

            if (memory)
            {
                m_Allocator.freeTextureMemory(this);
               // memory = VkDeviceMemory();
            }
        }
	}


    TextureHandle Device::createHandleForNativeTexture(uint32_t objectType, RHIObject _texture, const TextureDesc& desc)
    {
        if (_texture.integer == 0)
            return nullptr;

        if (objectType != ObjectTypes::VK_Image)
            return nullptr;

        VkImage image;

        ETexture* texture = new ETexture(m_Context, m_Allocator);
        fillTextureInfo(texture, desc);

        texture->image = image;
        texture->managed = false;

        return TextureHandle::Create(texture);
    }

}

