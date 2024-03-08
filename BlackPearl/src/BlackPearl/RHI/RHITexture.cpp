#include "pch.h"
#include "RHITexture.h"
namespace BlackPearl {
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

}

