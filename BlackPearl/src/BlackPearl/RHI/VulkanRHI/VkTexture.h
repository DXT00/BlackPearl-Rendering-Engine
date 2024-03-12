#pragma once
#include "BlackPearl/RHI/RHITexture.h"
#include <vulkan/vulkan.h>

namespace BlackPearl {
    //先用ETexture定义，防止和OpenGL定义的Texture重复
    class ETexture : public RefCounter<ITexture>
    {
    public:

        TextureDesc desc;
        VkImageCreateInfo imageInfo;
        VkExternalMemoryImageCreateInfo externalMemoryImageInfo;
        VkImage image;
        VkImageView view;

        ~ETexture() override {}
        const TextureDesc& getDesc() const override { return desc; }
       

    private:
  
    private:
    };

}

