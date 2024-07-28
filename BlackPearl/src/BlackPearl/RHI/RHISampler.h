#pragma once
#include<string>
#include "RHIDefinitions.h"
#include "RefCountPtr.h"
#include "RHIResources.h"
namespace BlackPearl {
    //////////////////////////////////////////////////////////////////////////
// Sampler
//////////////////////////////////////////////////////////////////////////
    enum class FilterMode: uint8_t {
        Linear,
        Nearest,
        Nearest_Mip_Nearnest,
        Linear_Mip_Linear
    };

    enum class SamplerAddressMode: uint8_t
    {
        // D3D names
        Clamp,
        Wrap,
        Border,
        Mirror,
        MirrorOnce,

        // Vulkan names
        ClampToEdge = Clamp,
        Repeat = Wrap,
        ClampToBorder = Border,
        MirroredRepeat = Mirror,
        MirrorClampToEdge = MirrorOnce
    };

    enum class SamplerReductionType : uint8_t
    {
        Standard,
        Comparison,
        Minimum,
        Maximum
    };

    struct SamplerDesc
    {
        Color borderColor = 1.f;
        float maxAnisotropy = 1.f;
        float mipBias = 0.f;

        bool minFilter = true;
        bool magFilter = true;
        bool mipFilter = true;
        SamplerAddressMode addressU = SamplerAddressMode::Clamp;
        SamplerAddressMode addressV = SamplerAddressMode::Clamp;
        SamplerAddressMode addressW = SamplerAddressMode::Clamp;
        SamplerReductionType reductionType = SamplerReductionType::Standard;

        SamplerDesc& setBorderColor(const Color& color) { borderColor = color; return *this; }
        SamplerDesc& setMaxAnisotropy(float value) { maxAnisotropy = value; return *this; }
        SamplerDesc& setMipBias(float value) { mipBias = value; return *this; }
        SamplerDesc& setMinFilter(bool enable) { minFilter = enable; return *this; }
        SamplerDesc& setMagFilter(bool enable) { magFilter = enable; return *this; }
        SamplerDesc& setMipFilter(bool enable) { mipFilter = enable; return *this; }
        SamplerDesc& setAllFilters(bool enable) { minFilter = magFilter = mipFilter = enable; return *this; }
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
