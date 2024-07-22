#pragma once
#if GE_API_VULKAN

#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/RHIHeap.h"
#include "BlackPearl/RHI/RHIState.h"
#include "VkMemoryResource.h"
#include <vulkan/vulkan.h>
#include <mutex>
#include <tuple>
#include "VkAllocator.h"
#include "VkContext.h"
namespace BlackPearl {
    struct TextureSubresourceView;

    //先用ETexture定义，防止和OpenGL定义的Texture重复
    class ETexture : public MemoryResource, public RefCounter<ITexture>, public TextureStateExtension
    {
    public:

        enum class TextureSubresourceViewType // see getSubresourceView()
        {
            AllAspects,
            DepthOnly,
            StencilOnly
        };
        struct Hash
        {
            std::size_t operator()(std::tuple<TextureSubresourceSet, TextureSubresourceViewType, TextureDimension, Format> const& s) const noexcept
            {
                const auto& [subresources, viewType, dimension, format] = s;

                size_t hash = 0;

                hash_combine(hash, subresources.baseMipLevel);
                hash_combine(hash, subresources.numMipLevels);
                hash_combine(hash, subresources.baseArraySlice);
                hash_combine(hash, subresources.numArraySlices);
                hash_combine(hash, viewType);
                hash_combine(hash, dimension);
                hash_combine(hash, format);

                return hash;
            }
        };
        TextureDesc desc;
        VkImageCreateInfo imageInfo;
        VkExternalMemoryImageCreateInfo externalMemoryImageInfo;
        VkImage image;
        VkImageView view;

        HeapHandle heap;

        void* sharedHandle = nullptr;

        // contains subresource views for this texture
        // note that we only create the views that the app uses, and that multiple views may map to the same subresources
        std::unordered_map<std::tuple<TextureSubresourceSet, TextureSubresourceViewType, TextureDimension, Format>, TextureSubresourceView, ETexture::Hash> subresourceViews;

        ETexture(const VulkanContext& context, VulkanAllocator& allocator)
            : TextureStateExtension(desc)
            , m_Context(context)
            , m_Allocator(allocator)
        { }

        // returns a subresource view for an arbitrary range of mip levels and array layers.
        // 'viewtype' only matters when asking for a depthstencil view; in situations where only depth or stencil can be bound
        // (such as an SRV with ImageLayout::eShaderReadOnlyOptimal), but not both, then this specifies which of the two aspect bits is to be set.
        TextureSubresourceView& getSubresourceView(const TextureSubresourceSet& subresources, TextureDimension dimension,
            Format format, TextureSubresourceViewType viewtype = TextureSubresourceViewType::AllAspects);

        uint32_t getNumSubresources() const;
        uint32_t getSubresourceIndex(uint32_t mipLevel, uint32_t arrayLayer) const;

        ~ETexture() override;
        const TextureDesc& getDesc() const override { return desc; }
        virtual const Type& GetType() const override { return desc.type; }
       // Object getNativeObject(ObjectType objectType) override;
       // Object getNativeView(ObjectType objectType, Format format, TextureSubresourceSet subresources, TextureDimension dimension, bool isReadOnlyDSV = false) override;

    private:
        const VulkanContext& m_Context;
        VulkanAllocator m_Allocator;
        std::mutex m_Mutex;


    };

    struct TextureSubresourceView
    {
        ETexture& texture;
        TextureSubresourceSet subresource;

        VkImageView view = nullptr;
        VkImageSubresourceRange subresourceRange;

        TextureSubresourceView(ETexture& texture)
            : texture(texture)
        { }

        TextureSubresourceView(const TextureSubresourceView&) = delete;

        bool operator==(const TextureSubresourceView& other) const
        {
            return &texture == &other.texture &&
                subresource == other.subresource &&
                view == other.view &&
                subresourceRange.aspectMask == other.subresourceRange.aspectMask 
                && subresourceRange.baseArrayLayer == other.subresourceRange.baseArrayLayer
                && subresourceRange.baseMipLevel == other.subresourceRange.baseMipLevel
                && subresourceRange.layerCount == other.subresourceRange.layerCount
                && subresourceRange.levelCount == other.subresourceRange.levelCount;
        }
    };

}

#endif