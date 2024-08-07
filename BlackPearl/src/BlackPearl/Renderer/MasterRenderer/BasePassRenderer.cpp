#include "pch.h"
#include "BasePassRenderer.h"
#include "BlackPearl/RHI/RHIInputLayout.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"
#include "BlackPearl/Renderer/Material/MaterialBindingCache.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "hlsl/core/forward_cb.h"
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

            m_MaterialBindings = CreateMaterialBindingCache();
            //GE_ASSERT(0, "params.materialBindings = nullptr");

        }

        auto samplerDesc = SamplerDesc()
            .setAllAddressModes(SamplerAddressMode::Border)
            .setBorderColor(1.0f);
        m_ShadowSampler = m_Device->createSampler(samplerDesc);

        //constant uniform variables
        m_ForwardViewCB = m_Device->createBuffer(RHIUtils::CreateVolatileConstantBufferDesc(sizeof(ForwardShadingViewConstants), "ForwardShadingViewConstants", params.numConstantBufferVersions));
        m_ForwardLightCB = m_Device->createBuffer(RHIUtils::CreateVolatileConstantBufferDesc(sizeof(ForwardShadingLightConstants), "ForwardShadingLightConstants", params.numConstantBufferVersions));

        m_ViewBindingLayout = CreateViewBindingLayout();
        m_ViewBindingSet = CreateViewBindingSet();
        m_LightBindingLayout = CreateLightBindingLayout();

        m_DrawStrategy = DBG_NEW InstancedOpaqueDrawStrategy();

        m_CommonPasses = std::make_shared<CommonRenderPasses>(device, shaderFactory);

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
       // auto& context = static_cast<Context&>(abstractContext);

        ForwardShadingViewConstants viewConstants = {};
        view->FillPlanarViewConstants(viewConstants. view);
        commandList->writeBuffer(m_ForwardViewCB, &viewConstants, sizeof(viewConstants));

      /*  context.keyTemplate.bits.frontCounterClockwise = view->IsMirrored();
        context.keyTemplate.bits.reverseDepth = view->IsReverseDepth();*/


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

        //TODO:: ¼Ópass context
        PipelineKey key;// = context.keyTemplate;
        key.value = 0;
        key.bits.cullMode = cullMode;
        key.bits.domain = material->domain;
        key.bits.frontCounterClockwise = true;
        key.bits.reverseDepth = false;

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
    void BasePassRenderer::SetupInputBuffers(BufferGroup* buffers, GraphicsState& state)
    {

        buffers->indexBuffer = m_Device->createBuffer(buffers->indexBufferDesc);
        buffers->vertexBuffer = m_Device->createBuffer(buffers->vertexBufferDesc);

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

    /// <summary>
    /// Prepares the lights.
    /// </summary>
    /// <param name="context">The context.</param>
    /// <param name="commandList">The command list.</param>
    /// <param name="lights">The lights.</param>
    /// <param name="ambientColorTop">The ambient color top.</param>
    /// <param name="ambientColorBottom">The ambient color bottom.</param>
    /// <param name="lightProbes">The light probes.</param>
    void BasePassRenderer::PrepareLights(ICommandList* commandList, LightSources* lightSouces, math::float3 ambientColorTop, math::float3 ambientColorBottom, const std::vector<std::shared_ptr<LightProbe>>& lightProbes)
    {
        ITexture* shadowMapTexture = nullptr;
        int2 shadowMapTextureSize = 0;
        
        for (const auto& lightObj : lightSouces->GetParallelLights())
        {
           Light* light =  lightObj->GetComponent<ParallelLight>();
            if (light->shadowMap)
            {
                shadowMapTexture = light->shadowMap->GetTexture();
                shadowMapTextureSize = light->shadowMap->GetTextureSize();
                break;
            }
        }

        ITexture* lightProbeDiffuse = nullptr;
        ITexture* lightProbeSpecular = nullptr;
        ITexture* lightProbeEnvironmentBrdf = nullptr;

        for (const auto& probe : lightProbes)
        {
            if (!probe->enabled)
                continue;

            if (lightProbeDiffuse == nullptr || lightProbeSpecular == nullptr || lightProbeEnvironmentBrdf == nullptr)
            {
                lightProbeDiffuse = nullptr;// probe->diffuseMap;
                lightProbeSpecular = nullptr;//probe->specularMap;
                lightProbeEnvironmentBrdf = nullptr;//probe->environmentBrdf;
            }
            else
            {
               /* if (lightProbeDiffuse != probe->diffuseMap || lightProbeSpecular != probe->specularMap || lightProbeEnvironmentBrdf != probe->environmentBrdf)
                {
                    GE_CORE_ERROR("All lights probe submitted to ForwardShadingPass::PrepareLights(...) must use the same set of textures");
                    return;
                }*/
            }
        }

        {
            std::lock_guard<std::mutex> lockGuard(m_Mutex);

            BindingSetHandle& lightBindings = m_LightBindingSets[std::make_pair(shadowMapTexture, lightProbeDiffuse)];

            if (!lightBindings)
            {
                lightBindings = CreateLightBindingSet(shadowMapTexture, lightProbeDiffuse, lightProbeSpecular, lightProbeEnvironmentBrdf);
            }

           // context.lightBindingSet = lightBindings;
        }

        ForwardShadingLightConstants constants = {};

        constants.shadowMapTextureSize = float2(shadowMapTextureSize);
        constants.shadowMapTextureSizeInv = 1.f / constants.shadowMapTextureSize;

        int numShadows = 0;
        std::vector<Light*> lights = lightSouces->GetLights();
        for (int nLight = 0; nLight < std::min(static_cast<int>(lights.size()), FORWARD_MAX_LIGHTS); nLight++)
        {
            const auto& light = lights[nLight];

            LightConstants& lightConstants = constants.lights[constants.numLights];
            light->FillLightConstants(lightConstants);

            if (light->shadowMap)
            {
                for (uint32_t cascade = 0; cascade < light->shadowMap->GetNumberOfCascades(); cascade++)
                {
                    if (numShadows < FORWARD_MAX_SHADOWS)
                    {
                        light->shadowMap->GetCascade(cascade)->FillShadowConstants(constants.shadows[numShadows]);
                        lightConstants.shadowCascades[cascade] = numShadows;
                        ++numShadows;
                    }
                }

                for (uint32_t perObjectShadow = 0; perObjectShadow < light->shadowMap->GetNumberOfPerObjectShadows(); perObjectShadow++)
                {
                    if (numShadows < FORWARD_MAX_SHADOWS)
                    {
                        light->shadowMap->GetPerObjectShadow(perObjectShadow)->FillShadowConstants(constants.shadows[numShadows]);
                        lightConstants.perObjectShadows[perObjectShadow] = numShadows;
                        ++numShadows;
                    }
                }
            }

            ++constants.numLights;
        }

        constants.ambientColorTop = float4(ambientColorTop, 0.f);
        constants.ambientColorBottom = float4(ambientColorBottom, 0.f);

        for (const auto& probe : lightProbes)
        {
            if (!probe->enabled)
                continue;

            LightProbeConstants& lightProbeConstants = constants.lightProbes[constants.numLightProbes];
            probe->FillLightProbeConstants(lightProbeConstants);

            ++constants.numLightProbes;

            if (constants.numLightProbes >= FORWARD_MAX_LIGHT_PROBES)
                break;
        }

        commandList->writeBuffer(m_ForwardLightCB, &constants, sizeof(constants));


    }


    ShaderHandle BasePassRenderer::CreateVertexShader(const std::shared_ptr<ShaderFactory>& shaderFactory, const CreateParameters& params)
    {
        return shaderFactory->CreateShader("hlsl/test/forward_test_vs.hlsl", "main", nullptr, ShaderType::Vertex);
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

        return shaderFactory->CreateShader("hlsl/test/forward_test_ps.hlsl", "main", &Macros, ShaderType::Pixel);
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
        RHIBindingLayoutDesc viewLayoutDesc;
        viewLayoutDesc.visibility = ShaderType::All;
        viewLayoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(1),
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(2),
            RHIBindingLayoutItem::RT_Sampler(1)
        };                        

        return m_Device->createBindingLayout(viewLayoutDesc);
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
        RHIBindingLayoutDesc lightProbeBindingDesc;
        lightProbeBindingDesc.visibility = ShaderType::Pixel;
        lightProbeBindingDesc.bindings = {
            RHIBindingLayoutItem::RT_Texture_SRV(10),
            RHIBindingLayoutItem::RT_Texture_SRV(11),
            RHIBindingLayoutItem::RT_Texture_SRV(12),
            RHIBindingLayoutItem::RT_Texture_SRV(13),
            RHIBindingLayoutItem::RT_Sampler(2),
            RHIBindingLayoutItem::RT_Sampler(3)
        };

        return m_Device->createBindingLayout(lightProbeBindingDesc);
    }

    BindingSetHandle BasePassRenderer::CreateLightBindingSet(ITexture* shadowMapTexture, ITexture* diffuse, ITexture* specular, ITexture* environmentBrdf)
    {
        BindingSetDesc bindingSetDesc;

        bindingSetDesc.bindings = {
            BindingSetItem::Texture_SRV(10, shadowMapTexture ? shadowMapTexture : m_CommonPasses->m_BlackTexture2DArray.Get()),
            BindingSetItem::Texture_SRV(11, diffuse ? diffuse : m_CommonPasses->m_BlackCubeMapArray.Get()),
            BindingSetItem::Texture_SRV(12, specular ? specular : m_CommonPasses->m_BlackCubeMapArray.Get()),
            BindingSetItem::Texture_SRV(13, environmentBrdf ? environmentBrdf : m_CommonPasses->m_BlackTexture.Get()),
            BindingSetItem::Sampler(2, m_CommonPasses->m_LinearWrapSampler),
            BindingSetItem::Sampler(3, m_CommonPasses->m_LinearClampSampler)
        };
        bindingSetDesc.trackLiveness = m_TrackLiveness;

        return m_Device->createBindingSet(bindingSetDesc, m_LightBindingLayout);
    }

    std::shared_ptr<MaterialBindingCache> BasePassRenderer::CreateMaterialBindingCache()
    {
        std::vector<MaterialResourceBinding> materialBindings = {
               { MaterialResource::ConstantBuffer, 0 },
               { MaterialResource::DiffuseTexture, 0 },
               { MaterialResource::SpecularTexture, 1 },
               { MaterialResource::NormalTexture, 2 },
               { MaterialResource::EmissiveTexture, 3 },
               { MaterialResource::OcclusionTexture, 4 },
               { MaterialResource::TransmissionTexture, 5 },
               { MaterialResource::Sampler, 0 },
        };

        return std::make_shared<MaterialBindingCache>(
            m_Device,
            ShaderType::Pixel,
            /* registerSpace = */ 0,
            materialBindings,
            nullptr,//commonPasses.m_AnisotropicWrapSampler,
            nullptr,//commonPasses.m_GrayTexture,
            nullptr//commonPasses.m_BlackTexture
        );
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