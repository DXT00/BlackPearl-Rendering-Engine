#pragma once
#include "ShadowMap.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
namespace BlackPearl {
    using namespace math;
	class ShadowMap2D: public IShadowMap
	{
    private:
        TextureHandle m_ShadowMapTexture;
        std::shared_ptr<IView> m_View;
        bool m_IsLitOutOfBounds = false;
        float2 m_FadeRangeTexels = 1.f;
        float2 m_ShadowMapSize;
        float2 m_TextureSize;
        float m_FalloffDistance = 1.f;

    public:
        ShadowMap2D(
            IDevice* device,
            int resolution,
            Format format);

        ShadowMap2D(
            IDevice* device,
            ITexture* texture,
            uint32_t arraySlice,
            const RHIViewport& viewport);

        bool SetupWholeSceneDirectionalLightView(
            const ParallelLight& light,
            box3_arg sceneBounds,
            float fadeRangeWorld = 0.f);

        bool SetupDynamicDirectionalLightView(
            const ParallelLight& light,
            float3 anchor,
            float3 halfShadowBoxSize,
            float3 preViewTranslation = 0.f,
            float fadeRangeWorld = 0.f);

        void SetupProxyView();

        void Clear(ICommandList* commandList);

        void SetLitOutOfBounds(bool litOutOfBounds);
        void SetFalloffDistance(float distance);

        std::shared_ptr<SceneData> GetPlanarView();

        virtual float4x4 GetWorldToUvzwMatrix() const override;
        virtual const IView& GetView() const override;
        virtual ITexture* GetTexture() const override;
        virtual uint32_t GetNumberOfCascades() const override;
        virtual const IShadowMap* GetCascade(uint32_t index) const override;
        virtual uint32_t GetNumberOfPerObjectShadows() const override;
        virtual const IShadowMap* GetPerObjectShadow(uint32_t index) const override;
        virtual int2 GetTextureSize() const override;
        virtual box2 GetUVRange() const override;
        virtual float2 GetFadeRangeInTexels() const override;
        virtual bool IsLitOutOfBounds() const override;
        virtual void FillShadowConstants(ShadowConstants& constants) const override;

	};
}

