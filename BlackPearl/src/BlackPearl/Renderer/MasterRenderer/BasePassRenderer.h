#pragma once
#include "BlackPearl/Renderer/MasterRenderer/GeometryRenderer.h"
#include "BlackPearl/Renderer/Material/MaterialBindingCache.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHICommandList.h"
#include "BlackPearl/Renderer/BindingCache.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/Renderer/SceneType.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Math/Math.h"
#include "BlackPearl/Renderer/CommonRenderPass.h"
namespace std
{
    template<>
    struct hash<std::pair<BlackPearl::ITexture*, BlackPearl::ITexture*>>
    {
        size_t operator()(const std::pair<BlackPearl::ITexture*, BlackPearl::ITexture*>& v) const noexcept
        {
            auto h = hash<BlackPearl::ITexture*>();
            return h(v.first) ^ (h(v.second) << 8);
        }
    };
}

namespace BlackPearl {


  
	class BasePassRenderer : public GeometryRenderer
	{
    public:
        struct CreateParameters
        {
            std::shared_ptr<MaterialBindingCache> materialBindings;
            bool singlePassCubemap = false;
            bool trackLiveness = true;
            uint32_t numConstantBufferVersions = 16;
        };
        union PipelineKey
        {
            struct
            {
                MaterialDomain domain : 3;
                RasterCullMode cullMode : 2;
                bool frontCounterClockwise : 1;
                bool reverseDepth : 1;
            } bits;
            uint32_t value = 0;

            static constexpr size_t Count = 1 << 7;
        };

        BasePassRenderer();
        virtual ~BasePassRenderer();

        //PostProcessRenderer(
        //    IDevice* device
        //    //std::shared_ptr<donut::ShaderFactory> shaderFactory,
        //    //std::shared_ptr<donut::CommonRenderPasses> commonPasses
        //    //, std::shared_ptr<FramebufferFactory> colorFramebufferFactory
        //);

        void Init(IDevice* device, const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params);

        void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);
        void SetupView(ICommandList* commandList, const IView* view, const IView* viewPrev) override;
        bool SetupMaterial(const Material* material, RasterCullMode cullMode, GraphicsState& state) override;
        void SetupInputBuffers(BufferGroup* buffers, GraphicsState& state) override;
        void SetPushConstants(ICommandList* commandList, GraphicsState& state, DrawArguments& args) override { }
        void PrepareLights(
            ICommandList* commandList,
            LightSources* lights,
            math::float3 ambientColorTop,
            math::float3 ambientColorBottom,
            const std::vector<std::shared_ptr<LightProbe>>& lightProbes);

    protected:
        InstancedOpaqueDrawStrategy* m_DrawStrategy;

        DeviceHandle m_Device;
        InputLayoutHandle m_InputLayout;
        ShaderHandle m_VertexShader;
        ShaderHandle m_PixelShader;
        ShaderHandle m_PixelShaderTransmissive;
        ShaderHandle m_GeometryShader;
        SamplerHandle m_ShadowSampler;
        BindingLayoutHandle m_ViewBindingLayout;
        BindingSetHandle m_ViewBindingSet;
        BindingLayoutHandle m_LightBindingLayout;
        //ViewType::Enum m_SupportedViewTypes = ViewType::PLANAR;
        BufferHandle m_ForwardViewCB;
        BufferHandle m_ForwardLightCB;
        GraphicsPipelineHandle m_Pipelines[PipelineKey::Count];
        bool m_TrackLiveness = true;
        std::mutex m_Mutex;

        std::unordered_map<std::pair<ITexture*, ITexture*>, BindingSetHandle> m_LightBindingSets;

        std::shared_ptr<CommonRenderPasses> m_CommonPasses;
        std::shared_ptr<MaterialBindingCache> m_MaterialBindings;

        virtual ShaderHandle CreateVertexShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params);
        virtual ShaderHandle CreateGeometryShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params);
        virtual ShaderHandle CreatePixelShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params, bool transmissiveMaterial);
        virtual InputLayoutHandle CreateInputLayout(IShader* vertexShader, const CreateParameters& params);
        virtual BindingLayoutHandle CreateViewBindingLayout();
        virtual BindingSetHandle CreateViewBindingSet();
        virtual BindingLayoutHandle CreateLightBindingLayout();
        virtual BindingSetHandle CreateLightBindingSet(ITexture* shadowMapTexture, ITexture* diffuse, ITexture* specular, ITexture* environmentBrdf);
        virtual std::shared_ptr<MaterialBindingCache> CreateMaterialBindingCache();
        virtual GraphicsPipelineHandle CreateGraphicsPipeline(PipelineKey key, IFramebuffer* framebuffer);

   
	};


}
