#include "pch.h"
#include "BasePassRenderer.h"
#include "BlackPearl/RHI/RHIInputLayout.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"

namespace BlackPearl {
    BasePassRenderer::BasePassRenderer()
    {
    }
    BasePassRenderer::~BasePassRenderer()
    {
    }
    void BasePassRenderer::Init(IDevice* device, const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params)
    {
        m_Device = device;
        std::vector<ShaderMacro> shaderMacros;
        m_VertexShader = CreateVertexShader(shaderFactory, params);
        m_InputLayout = CreateInputLayout(m_VertexShader, params);
        m_GeometryShader = CreateGeometryShader(shaderFactory, params);
        m_PixelShader = CreatePixelShader(shaderFactory, params, false);
        m_PixelShaderTransmissive = CreatePixelShader(shaderFactory, params, true);

        if (params.materialBindings)
            m_MaterialBindings = params.materialBindings;
        else {
            //TODO::

            //m_MaterialBindings = CreateMaterialBindingCache(*m_CommonPasses);
            GE_ASSERT(0, "params.materialBindings = nullptr");

        }

        auto samplerDesc = SamplerDesc()
            .setAllAddressModes(SamplerAddressMode::Border)
            .setBorderColor(1.0f);
        m_ShadowSampler = m_Device->createSampler(samplerDesc);

        //TODO::
        //m_ForwardViewCB = m_Device->createBuffer(RHIUtils::CreateVolatileConstantBufferDesc(sizeof(ForwardShadingViewConstants), "ForwardShadingViewConstants", params.numConstantBufferVersions));
        //m_ForwardLightCB = m_Device->createBuffer(RHIUtils::CreateVolatileConstantBufferDesc(sizeof(ForwardShadingLightConstants), "ForwardShadingLightConstants", params.numConstantBufferVersions));

        m_ViewBindingLayout = CreateViewBindingLayout();
        m_ViewBindingSet = CreateViewBindingSet();
        m_LightBindingLayout = CreateLightBindingLayout();

        m_DrawStrategy = DBG_NEW InstancedOpaqueDrawStrategy();


    }
    void BasePassRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        commandList->beginMarker("BasePass");
        SceneData* view = Renderer::GetSceneData();
        SceneData* preView = Renderer::GetPreSceneData();

        m_DrawStrategy->PrepareForView(scene, *view);
        RenderView(commandList, targetFramebuffer, view, preView, m_DrawStrategy, this);
        commandList->endMarker();
   
    }
    void BasePassRenderer::SetupView(ICommandList* commandList, const IView* view, const IView* viewPrev)
    {
    }
    bool BasePassRenderer::SetupMaterial(const Material* material, RasterCullMode cullMode, GraphicsState& state)
    {
       // auto& context = static_cast<Context&>(abstractContext);

        IBindingSet* materialBindingSet = m_MaterialBindings->GetMaterialBindingSet(material);

        if (!materialBindingSet)
            return false;

        if (material->domain >= MaterialDomain::Count || cullMode > RasterCullMode::None)
        {
            assert(false);
            return false;
        }

        PipelineKey key;// = context.keyTemplate;
        key.bits.cullMode = cullMode;
        key.bits.domain = material->domain;

        GraphicsPipelineHandle& pipeline = m_Pipelines[key.value];

        if (!pipeline)
        {
            std::lock_guard<std::mutex> lockGuard(m_Mutex);

            if (!pipeline)
                pipeline = CreateGraphicsPipeline(key, state.framebuffer);

            if (!pipeline)
                return false;
        }

        assert(pipeline->getFramebufferInfo() == state.framebuffer->getFramebufferInfo());

        state.pipeline = pipeline;
        //state.bindings = { materialBindingSet, m_ViewBindingSet, context.lightBindingSet };
        state.bindings = { materialBindingSet, m_ViewBindingSet };


        return true;
    }
    void BasePassRenderer::SetupInputBuffers(const BufferGroup* buffers, GraphicsState& state)
    {
        state.vertexBuffers = {
        { buffers->vertexBuffer, 0, buffers->getVertexBufferRange(VertexAttribute::Position).byteOffset },
        { buffers->vertexBuffer, 1, buffers->getVertexBufferRange(VertexAttribute::PrevPosition).byteOffset },
        { buffers->vertexBuffer, 2, buffers->getVertexBufferRange(VertexAttribute::TexCoord1).byteOffset },
        { buffers->vertexBuffer, 3, buffers->getVertexBufferRange(VertexAttribute::Normal).byteOffset },
        { buffers->vertexBuffer, 4, buffers->getVertexBufferRange(VertexAttribute::Tangent).byteOffset },
        { buffers->instanceBuffer, 5, 0 }
        };

        state.indexBuffer = { buffers->indexBuffer, Format::R32_UINT, 0 };
    }


    ShaderHandle BasePassRenderer::CreateVertexShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params)
    {
        return shaderFactory->CreateShader("hlsl/passes/forward_vs.hlsl", "main", nullptr, ShaderType::Vertex);
    }

    ShaderHandle BasePassRenderer::CreateGeometryShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params)
    {
       /* if (params.singlePassCubemap)
        {
            ShaderDesc desc(ShaderType::Geometry);
            desc.fastGSFlags = FastGeometryShaderFlags(
                FastGeometryShaderFlags::ForceFastGS |
                FastGeometryShaderFlags::UseViewportMask |
                FastGeometryShaderFlags::OffsetTargetIndexByViewportIndex);

            desc.pCoordinateSwizzling = CubemapView::GetCubemapCoordinateSwizzle();

            return shaderFactory.CreateShader("donut/passes/cubemap_gs.hlsl", "main", nullptr, desc);
        }*/

        return nullptr;
    }

    ShaderHandle BasePassRenderer::CreatePixelShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params, bool transmissiveMaterial)
    {
        std::vector<ShaderMacro> Macros;
        Macros.push_back(ShaderMacro("TRANSMISSIVE_MATERIAL", transmissiveMaterial ? "1" : "0"));

        return shaderFactory->CreateShader("assets/passes/forward_ps.hlsl", "main", &Macros, ShaderType::Pixel);
    }

    InputLayoutHandle BasePassRenderer::CreateInputLayout(IShader* vertexShader, const CreateParameters& params)
    {
        const VertexAttributeDesc inputDescs[] =
        {
            GetVertexAttributeDesc(VertexAttribute::Position, "POS", 0),
            GetVertexAttributeDesc(VertexAttribute::PrevPosition, "PREV_POS", 1),
            GetVertexAttributeDesc(VertexAttribute::TexCoord1, "TEXCOORD", 2),
            GetVertexAttributeDesc(VertexAttribute::Normal, "NORMAL", 3),
            GetVertexAttributeDesc(VertexAttribute::Tangent, "TANGENT", 4),
            GetVertexAttributeDesc(VertexAttribute::Transform, "TRANSFORM", 5),
        };

        return m_Device->createInputLayout(inputDescs, uint32_t(std::size(inputDescs)), vertexShader);
    }




    BindingLayoutHandle BasePassRenderer::CreateViewBindingLayout()
    {
        return BindingLayoutHandle();
    }

    BindingSetHandle BasePassRenderer::CreateViewBindingSet()
    {
       BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::ConstantBuffer(1, m_ForwardViewCB),
            BindingSetItem::ConstantBuffer(2, m_ForwardLightCB),
            BindingSetItem::Sampler(1, m_ShadowSampler)
        };
        bindingSetDesc.trackLiveness = m_TrackLiveness;

        return m_Device->createBindingSet(bindingSetDesc, m_ViewBindingLayout);
    }

    BindingLayoutHandle BasePassRenderer::CreateLightBindingLayout()
    {
        return BindingLayoutHandle();
    }

    BindingSetHandle BasePassRenderer::CreateLightBindingSet(ITexture* shadowMapTexture, ITexture* diffuse, ITexture* specular, ITexture* environmentBrdf)
    {
        return BindingSetHandle();
    }

    GraphicsPipelineHandle BasePassRenderer::CreateGraphicsPipeline(PipelineKey key, IFramebuffer* framebuffer)
    {
        GraphicsPipelineDesc pipelineDesc;
        pipelineDesc.inputLayout = m_InputLayout;
        pipelineDesc.VS = m_VertexShader;
        pipelineDesc.GS = m_GeometryShader;
        pipelineDesc.renderState.rasterState.frontCounterClockwise = key.bits.frontCounterClockwise;
        pipelineDesc.renderState.rasterState.setCullMode(key.bits.cullMode);
        pipelineDesc.renderState.blendState.alphaToCoverageEnable = false;
        pipelineDesc.bindingLayouts = { m_MaterialBindings->GetLayout(), m_ViewBindingLayout, m_LightBindingLayout };

        pipelineDesc.renderState.depthStencilState
            .setDepthFunc(key.bits.reverseDepth
                ? ComparisonFunc::GreaterOrEqual
                : ComparisonFunc::LessOrEqual);

        switch (key.bits.domain)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case MaterialDomain::Opaque:
            pipelineDesc.PS = m_PixelShader;
            break;

        case MaterialDomain::AlphaTested:
            pipelineDesc.PS = m_PixelShader;
            pipelineDesc.renderState.blendState.alphaToCoverageEnable = true;
            break;

        case MaterialDomain::AlphaBlended: {
            pipelineDesc.PS = m_PixelShader;
            pipelineDesc.renderState.blendState.alphaToCoverageEnable = false;
            pipelineDesc.renderState.blendState.targets[0]
                .enableBlend()
                .setSrcBlend(BlendFactor::SrcAlpha)
                .setDestBlend(BlendFactor::InvSrcAlpha)
                .setSrcBlendAlpha(BlendFactor::Zero)
                .setDestBlendAlpha(BlendFactor::One);

            pipelineDesc.renderState.depthStencilState.disableDepthWrite();
            break;
        }

        case MaterialDomain::Transmissive:
        case MaterialDomain::TransmissiveAlphaTested:
        case MaterialDomain::TransmissiveAlphaBlended: {
            pipelineDesc.PS = m_PixelShaderTransmissive;
            pipelineDesc.renderState.blendState.alphaToCoverageEnable = false;
            pipelineDesc.renderState.blendState.targets[0]
                .enableBlend()
                .setSrcBlend(BlendFactor::One)
                .setDestBlend(BlendFactor::Src1Color)
                .setSrcBlendAlpha(BlendFactor::Zero)
                .setDestBlendAlpha(BlendFactor::One);

            pipelineDesc.renderState.depthStencilState.disableDepthWrite();
            break;
        }
        default:
            return nullptr;
        }

        return m_Device->createGraphicsPipeline(pipelineDesc, framebuffer);
    }

}