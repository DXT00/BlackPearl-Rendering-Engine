#pragma once
#include "ShadowMap.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
namespace BlackPearl {
    class ShadowMap2D;

    class CascadedShadowMap : public IShadowMap
    {
    private:
        TextureHandle m_ShadowMapTexture;
        std::vector<std::shared_ptr<ShadowMap2D>> m_Cascades;
        std::vector<std::shared_ptr<ShadowMap2D>> m_PerObjectShadows;
        std::vector<std::shared_ptr<SceneData>> m_CompositeView;
        int m_NumberOfCascades;

    public:
        CascadedShadowMap(
            IDevice* device,
            int resolution,
            int numCascades,
            int numPerObjectShadows,
            Format format,
            bool isUAV = false);

        // Computes the cascade projections based on the view frustum, shadow distance, and the distribution exponent.
        bool SetupForPlanarView(
            const ParallelLight& light,
            frustum viewFrustum,
            float maxShadowDistance,
            float lightSpaceZUp,
            float lightSpaceZDown,
            float exponent = 4.f,
            float3 preViewTranslation = 0.f,
            int numberOfCascades = -1);

        // Similar to SetupForPlanarView, but the size of the cascades does not depend on orientation, and therefore 
        // the shadow map texels have the same world space projections when the camera turns or moves.
        // The downside of this algorithm is that the cascades are often larger than necessary.
        bool SetupForPlanarViewStable(
            const ParallelLight& light,
            frustum projectionFrustum,
            affine3 inverseViewMatrix,
            float maxShadowDistance,
            float lightSpaceZUp,
            float lightSpaceZDown,
            float exponent = 4.f,
            float3 preViewTranslation = 0.f,
            int numberOfCascades = -1);

        // Computes the cascade projections to cover an omnidirectional view from a given point. The cascades are all centered on that point.
        bool SetupForCubemapView(
            const ParallelLight& light,
            float3 center,
            float maxShadowDistance, float lightSpaceZUp,
            float lightSpaceZDown,
            float exponent = 4.f,
            int numberOfCascades = -1);

        // Computes a simple directional shadow projection that covers a given world space box.
        bool SetupPerObjectShadow(const ParallelLight& light, uint32_t object, const box3& objectBounds);

        void SetupProxyViews();

        void Clear(ICommandList* commandList);

        void SetLitOutOfBounds(bool litOutOfBounds);
        void SetFalloffDistance(float distance);
        void SetNumberOfCascadesUnsafe(int cascades);

        std::shared_ptr<IView> GetCascadeView(uint32_t cascade);
        std::shared_ptr<IView> GetPerObjectView(uint32_t object);

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

