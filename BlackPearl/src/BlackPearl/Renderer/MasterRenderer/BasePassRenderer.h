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
namespace BlackPearl {


    enum class MaterialDomain : uint8_t
    {
        Opaque,
        AlphaTested,
        AlphaBlended,
        Transmissive,
        TransmissiveAlphaTested,
        TransmissiveAlphaBlended,

        Count
    };

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
            uint32_t value;

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
        void SetupInputBuffers(const BufferGroup* buffers, GraphicsState& state) override;
        void SetPushConstants(ICommandList* commandList, GraphicsState& state, DrawArguments& args) override { }


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

        //std::shared_ptr<CommonRenderPasses> m_CommonPasses;
        std::shared_ptr<MaterialBindingCache> m_MaterialBindings;

        virtual ShaderHandle CreateVertexShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params);
        virtual ShaderHandle CreateGeometryShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params);
        virtual ShaderHandle CreatePixelShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params, bool transmissiveMaterial);
        virtual InputLayoutHandle CreateInputLayout(IShader* vertexShader, const CreateParameters& params);
        virtual BindingLayoutHandle CreateViewBindingLayout();
        virtual BindingSetHandle CreateViewBindingSet();
        virtual BindingLayoutHandle CreateLightBindingLayout();
        virtual BindingSetHandle CreateLightBindingSet(ITexture* shadowMapTexture, ITexture* diffuse, ITexture* specular, ITexture* environmentBrdf);
        //virtual std::shared_ptr<MaterialBindingCache> CreateMaterialBindingCache(CommonRenderPasses& commonPasses);
        virtual GraphicsPipelineHandle CreateGraphicsPipeline(PipelineKey key, IFramebuffer* framebuffer);

   
	};


}