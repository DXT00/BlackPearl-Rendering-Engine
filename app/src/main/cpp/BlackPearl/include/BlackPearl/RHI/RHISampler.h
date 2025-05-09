#pragma once
#include<string>
#include "RHIDefinitions.h"
#include "RefCountPtr.h"
#include "RHIResources.h"
namespace BlackPearl {
    //////////////////////////////////////////////////////////////////////////
// Sampler
//////////////////////////////////////////////////////////////////////////


    struct SamplerDesc
    {
        Color borderColor = 1.f;
        float maxAnisotropy = 1.f;
        float mipBias = 0.f;

        FilterMode minFilter = FilterMode::Linear;
        FilterMode magFilter = FilterMode::Linear;
        FilterMode mipFilter = FilterMode::Linear_Mip_Linear;
        SamplerAddressMode addressU = SamplerAddressMode::Clamp;
        SamplerAddressMode addressV = SamplerAddressMode::Clamp;
        SamplerAddressMode addressW = SamplerAddressMode::Clamp;
        SamplerReductionType reductionType = SamplerReductionType::Standard;

        SamplerDesc& setBorderColor(const Color& color) { borderColor = color; return *this; }
        SamplerDesc& setMaxAnisotropy(float value) { maxAnisotropy = value; return *this; }
        SamplerDesc& setMipBias(float value) { mipBias = value; return *this; }
        SamplerDesc& setMinFilter(bool enable) { (enable)? minFilter = FilterMode::Linear: minFilter = FilterMode::Nearest; return *this; }
        SamplerDesc& setMagFilter(bool enable) { (enable) ? magFilter = FilterMode::Linear: magFilter = FilterMode::Nearest; return *this; }
        SamplerDesc& setMipFilter(bool enable) { (enable) ? mipFilter = FilterMode::Linear_Mip_Linear: mipFilter = FilterMode::Nearest_Mip_Nearnest; return *this; }
        SamplerDesc& setAllFilters(bool enable) {
            setMinFilter(enable); 
            setMagFilter(enable);
            setMipFilter(enable);

            return *this; }
        SamplerDesc& setAddressU(SamplerAddressMode mode) { addressU = mode; return *this; }
        SamplerDesc& setAddressV(SamplerAddressMode mode) { addressV = mode; return *this; }
        SamplerDesc& setAddressW(SamplerAddressMode mode) { addressW = mode; return *this; }
        SamplerDesc& setAllAddressModes(SamplerAddressMode mode) { addressU = addressV = addressW = mode; return *this; }
        SamplerDesc& setReductionType(SamplerReductionType type) { reductionType = type; return *this; }
    };

    class ISampler : public IResource
    {
    public:
        [[nodiscard]] virtual const SamplerDesc& getDesc() const = 0;
    };

    typedef RefCountPtr<ISampler> SamplerHandle;
}
