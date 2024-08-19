#include "pch.h"
#include "RHITexture.h"
#include <BlackPearl/Core.h>
namespace BlackPearl {
    uint32_t TextureSubresourceSet::AllMipLevels = uint32_t(-1);
    uint32_t TextureSubresourceSet::AllArraySlices = uint32_t(-1);

    TextureSubresourceSet TextureSubresourceSet::resolve(const TextureDesc& desc, bool singleMipLevel) const
	{
        TextureSubresourceSet ret;
        ret.baseMipLevel = baseMipLevel;

        if (singleMipLevel)
        {
            ret.numMipLevels = 1;
        }
        else
        {
            int lastMipLevelPlusOne = std::min(baseMipLevel + numMipLevels, desc.mipLevels);
            ret.numMipLevels = uint32_t(std::max(0u, lastMipLevelPlusOne - baseMipLevel));
        }

        switch (desc.dimension)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case TextureDimension::Texture1DArray:
        case TextureDimension::Texture2DArray:
        case TextureDimension::TextureCube:
        case TextureDimension::TextureCubeArray:
        case TextureDimension::Texture2DMSArray: {
            ret.baseArraySlice = baseArraySlice;
            int lastArraySlicePlusOne = std::min(baseArraySlice + numArraySlices, desc.arraySize);
            ret.numArraySlices = uint32_t(std::max(0u, lastArraySlicePlusOne - baseArraySlice));
            break;
        }
        default:
            ret.baseArraySlice = 0;
            ret.numArraySlices = 1;
            break;
        }

        return ret;
	}

    bool TextureSubresourceSet::isEntireTexture(const TextureDesc& desc) const
    {
        if (baseMipLevel > 0u || baseMipLevel + numMipLevels < desc.mipLevels)
            return false;

        switch (desc.dimension)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case TextureDimension::Texture1DArray:
        case TextureDimension::Texture2DArray:
        case TextureDimension::TextureCube:
        case TextureDimension::TextureCubeArray:
        case TextureDimension::Texture2DMSArray:
            if (baseArraySlice > 0u || baseArraySlice + numArraySlices < desc.arraySize)
                return false;
        default:
            return true;
        }
    }

    TextureSlice TextureSlice::resolve(const TextureDesc& desc) const
    {
        TextureSlice ret(*this);

        GE_ASSERT(mipLevel < desc.mipLevels, "mipLevel invalid");

        if (width == uint32_t(-1))
            ret.width = std::max(desc.width >> mipLevel, 1u);

        if (height == uint32_t(-1))
            ret.height = std::max(desc.height >> mipLevel, 1u);

        if (depth == uint32_t(-1))
        {
            if (desc.dimension == TextureDimension::Texture3D)
                ret.depth = std::max(desc.depth >> mipLevel, 1u);
            else
                ret.depth = 1;
        }

        return ret;
    }

}

