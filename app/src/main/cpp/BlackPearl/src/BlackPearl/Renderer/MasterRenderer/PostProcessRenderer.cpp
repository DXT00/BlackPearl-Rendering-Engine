#include "pch.h"
#include "BlackPearl/Renderer/RenderTargets.h"
#include "PostProcessRenderer.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
namespace BlackPearl {
	PostProcessRenderer::PostProcessRenderer()
	{
	}
	PostProcessRenderer::~PostProcessRenderer()
	{
	}
	/*PostProcessRenderer::PostProcessRenderer(IDevice* device)
	{
	}*/
    void PostProcessRenderer::Init(IDevice* device, std::shared_ptr<ShaderFactory> shaderFactory)
    {
        m_Device = device;
        for (uint32_t i = 0; i < (uint32_t)RenderPassType::MaxCount; i++)
        {
            std::vector<ShaderMacro> shaderMacros;
            switch ((RenderPassType)i)
            {
                //case(RenderPassType::Debug_DenoiserUnfilteredRadiance): shaderMacros.push_back(ShaderMacro({  "NRD_SHOW_UNFILTERED_RADIANCE", "1" })); break;
            case(RenderPassType::Debug_BlendDebugViz): shaderMacros.push_back(ShaderMacro({ "BLEND_DEBUG_BUFFER", "1" })); break;
            default:;
            };
            m_RenderShaders[i] = shaderFactory->CreateShader("app/PostProcess.hlsl", "main", &shaderMacros, ShaderType::Pixel);
        }

        for (uint32_t i = 0; i < (uint32_t)ComputePassType::MaxCount; i++)
        {
            std::vector<ShaderMacro> shaderMacros;
            switch ((ComputePassType)i)
            {
            case(ComputePassType::StablePlanesDebugViz):
                shaderMacros.push_back(ShaderMacro({ "STABLE_PLANES_DEBUG_VIZ", "1" }));
                break;
            case(ComputePassType::RELAXDenoiserPrepareInputs):
                shaderMacros.push_back(ShaderMacro({ "DENOISER_PREPARE_INPUTS", "1" }));
                shaderMacros.push_back(ShaderMacro({ "USE_RELAX", "1" }));
                break;
            case(ComputePassType::REBLURDenoiserPrepareInputs):
                shaderMacros.push_back(ShaderMacro({ "DENOISER_PREPARE_INPUTS", "1" }));
                shaderMacros.push_back(ShaderMacro({ "USE_RELAX", "0" }));
                break;
            case(ComputePassType::RELAXDenoiserFinalMerge):
                shaderMacros.push_back(ShaderMacro({ "DENOISER_FINAL_MERGE", "1" }));
                shaderMacros.push_back(ShaderMacro({ "USE_RELAX", "1" }));
                break;
            case(ComputePassType::REBLURDenoiserFinalMerge):
                shaderMacros.push_back(ShaderMacro({ "DENOISER_FINAL_MERGE", "1" }));
                shaderMacros.push_back(ShaderMacro({ "USE_RELAX", "0" }));
                break;
            case(ComputePassType::DummyPlaceholder): shaderMacros.push_back(ShaderMacro({ "DUMMY_PLACEHOLDER_EFFECT", "1" })); break;
            };
            m_ComputeShaders[i] = shaderFactory->CreateShader("app/PostProcess.hlsl", "main", &shaderMacros, ShaderType::Compute);
        }
        //m_MainCS = shaderFactory->CreateShader("app/PostProcess.hlsl", "main", &std::vector<ShaderMacro>(1, ShaderMacro("USE_CS", "1")), nvrhi::ShaderType::Compute);

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility =ShaderType::Pixel;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(0),
            RHIBindingLayoutItem::RT_PushConstants(1, sizeof(SampleMiniConstants)),
            RHIBindingLayoutItem::RT_Texture_SRV(0),
            RHIBindingLayoutItem::RT_Texture_SRV(4),
            RHIBindingLayoutItem::RT_Texture_SRV(5),
            RHIBindingLayoutItem::RT_Sampler(0)
        };
        m_BindingLayoutPS = m_Device->createBindingLayout(layoutDesc);

        layoutDesc.visibility = ShaderType::Compute | ShaderType::Pixel;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(0),
            RHIBindingLayoutItem::RT_PushConstants(1, sizeof(SampleMiniConstants)),
            RHIBindingLayoutItem::RT_Texture_SRV(0),
            RHIBindingLayoutItem::RT_Texture_UAV(0),
            RHIBindingLayoutItem::RT_Texture_UAV(1),
            RHIBindingLayoutItem::RT_Texture_SRV(2),
            RHIBindingLayoutItem::RT_Texture_SRV(3),
            RHIBindingLayoutItem::RT_Texture_SRV(4),
            RHIBindingLayoutItem::RT_Texture_SRV(5),
            RHIBindingLayoutItem::RT_Texture_SRV(6),
            RHIBindingLayoutItem::RT_Texture_SRV(7),
            RHIBindingLayoutItem::RT_StructuredBuffer_SRV(10),
            RHIBindingLayoutItem::RT_Sampler(0)
        };
        m_BindingLayoutCS = m_Device->createBindingLayout(layoutDesc);

        SamplerDesc samplerDesc;
        samplerDesc.setBorderColor(Color(0.f));
        samplerDesc.setAllFilters(true);
        samplerDesc.setMipFilter(false);
        samplerDesc.setAllAddressModes(SamplerAddressMode::Wrap);
        m_LinearSampler = m_Device->createSampler(samplerDesc);

        samplerDesc.setAllFilters(false);
        m_PointSampler = m_Device->createSampler(samplerDesc);
    }
	void PostProcessRenderer::Render(ICommandList* commandList,
        RenderPassType passType, 
        BufferHandle consts, 
        SampleMiniConstants& miniConsts, 
        IFramebuffer* targetFramebuffer,
        RenderTargets& renderTargets, 
        ITexture* sourceTexture)
	{

        commandList->beginMarker("PostProcessPS");

        assert((uint32_t)passType >= 0 && passType < RenderPassType::MaxCount);
        uint32_t passIndex = (uint32_t)passType;

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
                BindingSetItem::ConstantBuffer(0, consts),
                BindingSetItem::PushConstants(1, sizeof(SampleMiniConstants)),
                BindingSetItem::Texture_SRV(0, (sourceTexture != nullptr) ? (sourceTexture) : (m_CommonPasses->m_WhiteTexture.Get())),
                //BindingSetItem::StructuredBuffer_SRV(1, renderTargets.DenoiserPixelDataBuffer),
                //BindingSetItem::Texture_SRV(4, renderTargets.OutputColor),
               // BindingSetItem::Texture_SRV(5, renderTargets.DebugVizOutput),
                BindingSetItem::Sampler(0, m_LinearSampler /*m_PointSampler*/)
        };

        BindingSetHandle bindingSet = m_BindingCache.GetOrCreateBindingSet(bindingSetDesc, m_BindingLayoutPS);

        if (m_RenderPSOs[passIndex] == nullptr)
        {
            GraphicsPipelineDesc pipelineDesc;
            pipelineDesc.bindingLayouts = { m_BindingLayoutPS };
            pipelineDesc.primType = PrimitiveType::TriangleStrip;
            pipelineDesc.VS = m_CommonPasses->m_FullscreenVS;
            pipelineDesc.PS = m_RenderShaders[passIndex];
            pipelineDesc.rasterState.setCullNone();
            pipelineDesc.depthStencilState.depthTestEnable = false;
            pipelineDesc.depthStencilState.stencilEnable = false;
            pipelineDesc.blendState.targets[0].enableBlend()
                .setSrcBlend(BlendFactor::SrcAlpha)
                .setDestBlend(BlendFactor::InvSrcAlpha)
                .setSrcBlendAlpha(BlendFactor::Zero)
                .setDestBlendAlpha(BlendFactor::One);
            m_RenderPSOs[passIndex] = m_Device->createGraphicsPipeline(pipelineDesc, targetFramebuffer);
        }

        GraphicsState state;
        state.pipeline = m_RenderPSOs[passIndex];
        state.framebuffer = targetFramebuffer;
        state.bindings = { bindingSet };
        ViewportState viewportState;
        auto desc = targetFramebuffer->getDesc().colorAttachments[0].texture->getDesc();
        viewportState.addViewport(RHIViewport((float)desc.width, (float)desc.height));
        viewportState.addScissorRect(RHIRect(desc.width, desc.height));
        state.viewport = viewportState;
        commandList->setGraphicsState(state);

        DrawArguments args;
        args.instanceCount = 1;
        args.vertexCount = 4;
        commandList->setPushConstants(&miniConsts, sizeof(miniConsts));
        commandList->draw(args);

        commandList->endMarker();



	}
}