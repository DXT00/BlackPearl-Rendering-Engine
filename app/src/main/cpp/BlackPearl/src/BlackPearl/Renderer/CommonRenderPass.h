#pragma once
#include <unordered_map>
#include "BlackPearl/RHI/RHI.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/Math/Math.h"
namespace BlackPearl {
    class BindingCache;
    class ShaderFactory;

    constexpr uint32_t c_MaxRenderPassConstantBufferVersions = 16;

    enum class BlitSampler
    {
        Point,
        Linear,
        Sharpen
    };

    struct BlitParameters
    {
        IFramebuffer* targetFramebuffer = nullptr;
        RHIViewport targetViewport;
        dm::box2 targetBox = dm::box2(0.f, 1.f);

        ITexture* sourceTexture = nullptr;
        uint32_t sourceArraySlice = 0;
        uint32_t sourceMip = 0;
        dm::box2 sourceBox = dm::box2(0.f, 1.f);

        BlitSampler sampler = BlitSampler::Linear;
        BlendState::RenderTarget blendState;
        Color blendConstantColor = Color(0.f);
    };

    class CommonRenderPasses
    {
    protected:
        DeviceHandle m_Device;

        struct PsoCacheKey
        {
            FramebufferInfo fbinfo;
            IShader* shader;
            BlendState::RenderTarget blendState;

            bool operator==(const PsoCacheKey& other) const { return fbinfo == other.fbinfo && shader == other.shader && blendState == other.blendState; }
            bool operator!=(const PsoCacheKey& other) const { return !(*this == other); }

            struct Hash
            {
                size_t operator ()(const PsoCacheKey& s) const
                {
                    size_t hash = 0;
                    hash_combine(hash, s.fbinfo);
                    hash_combine(hash, s.shader);
                    hash_combine(hash, s.blendState);
                    return hash;
                }
            };
        };

        std::unordered_map<PsoCacheKey, GraphicsPipelineHandle, PsoCacheKey::Hash> m_BlitPsoCache;

    public:
        ShaderHandle m_FullscreenVS;
        ShaderHandle m_FullscreenAtOneVS;
        ShaderHandle m_RectVS;
        ShaderHandle m_BlitPS;
        ShaderHandle m_BlitArrayPS;
        ShaderHandle m_SharpenPS;
        ShaderHandle m_SharpenArrayPS;

        TextureHandle m_BlackTexture;
        TextureHandle m_GrayTexture;
        TextureHandle m_WhiteTexture;
        TextureHandle m_BlackTexture2DArray;
        TextureHandle m_WhiteTexture2DArray;
        TextureHandle m_BlackCubeMapArray;

        SamplerHandle m_PointClampSampler;
        SamplerHandle m_LinearClampSampler;
        SamplerHandle m_LinearWrapSampler;
        SamplerHandle m_AnisotropicWrapSampler;

        BindingLayoutHandle m_BlitBindingLayout;

        CommonRenderPasses(IDevice* device, std::shared_ptr<ShaderFactory> shaderFactory);

        void BlitTexture(ICommandList* commandList, const BlitParameters& params, BindingCache* bindingCache = nullptr);

        // Simplified form of BlitTexture that blits the entire source texture, mip 0 slice 0, into the entire target framebuffer using a linear sampler.
        void BlitTexture(ICommandList* commandList, IFramebuffer* targetFramebuffer, ITexture* sourceTexture, BindingCache* bindingCache = nullptr);
    };
}

