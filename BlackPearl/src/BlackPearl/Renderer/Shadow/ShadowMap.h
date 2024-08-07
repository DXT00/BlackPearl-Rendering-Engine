#pragma once
#include "BlackPearl/Math/Math.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "hlsl/core/light_cb.h"
namespace BlackPearl {
    using namespace math;

    class IShadowMap
    {
    public:
        virtual float4x4 GetWorldToUvzwMatrix() const = 0;
        virtual const class IView& GetView() const = 0;
        virtual ITexture* GetTexture() const = 0;
        virtual uint32_t GetNumberOfCascades() const = 0;
        virtual const IShadowMap* GetCascade(uint32_t index) const = 0;
        virtual uint32_t GetNumberOfPerObjectShadows() const = 0;
        virtual const IShadowMap* GetPerObjectShadow(uint32_t index) const = 0;
        virtual int2 GetTextureSize() const = 0;
        virtual box2 GetUVRange() const = 0;
        virtual float2 GetFadeRangeInTexels() const = 0;
        virtual bool IsLitOutOfBounds() const = 0;
        virtual void FillShadowConstants(ShadowConstants& constants) const = 0;
    };
}

