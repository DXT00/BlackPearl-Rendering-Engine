#include "pch.h"
#include "RtxdiRenderer.h"
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"

namespace BlackPearl {
#define RTXDI_SCREEN_SPACE_GROUP_SIZE 8

    RtxdiRenderer::RtxdiRenderer(IDevice* device, std::shared_ptr<ShaderFactory> shaderFactory, std::shared_ptr<CommonRenderPasses> commonRenderPasses, BindingLayoutHandle bindlessLayout) :
        m_Device(device),
        m_ShaderFactory(shaderFactory),
        m_CommonPasses(commonRenderPasses),
        m_BindlessLayout(bindlessLayout)
    {
        _CreateRTBindingLayout();
        _CreateRTBindingSets();
        m_Device = device;
        m_RayTracePass = DBG_NEW RayTraceRenderer();
        m_BindlessLayout = bindlessLayout;


        std::vector<ShaderMacro> regirMacros;
        bool useRayQuery = false;
        m_RayTracePass->Init(device, *shaderFactory, "app/RTXDI/GenerateInitialSamples.hlsl",
            regirMacros, useRayQuery, RTXDI_SCREEN_SPACE_GROUP_SIZE, m_BindingLayout, m_ExtraBindingLayout, m_BindlessLayout);
    }
    void RtxdiRenderer::PrepareResources(
		CommandListHandle commandList,
		const RenderTargets& renderTargets, 
		const BindingLayoutHandle extraBindingLayout,
        Scene* scene)
	{
        m_Scene = scene;
	}
	void RtxdiRenderer::BeginFrame(CommandListHandle commandList, const RenderTargets& renderTargets, const BindingLayoutHandle extraBindingLayout, BindingSetHandle extraBindingSet)
	{
	}
	void RtxdiRenderer::Execute(CommandListHandle commandList, BindingSetHandle extraBindingSet, bool skipFinal)
	{
	}
	void RtxdiRenderer::ExecuteGI(CommandListHandle commandList, BindingSetHandle extraBindingSet, bool skipFinal)
	{
	}
	void RtxdiRenderer::ExecuteFusedDIGIFinal(CommandListHandle commandList, BindingSetHandle extraBindingSet)
	{
	}
	void RtxdiRenderer::EndFrame()
	{
	}

    void RtxdiRenderer::_ExecuteRayTracingPass(CommandListHandle& commandList,
        RayTraceRenderer& pass,
        const char* passName,
        int2 dispatchSize,
        IBindingSet* extraBindingSet /* = nullptr */)
    {

        commandList->beginMarker(passName);

        uint4 unusedPushConstants = { 0,0,0,0 };  // shared bindings require them
        pass.Render(commandList, dispatchSize.x, dispatchSize.y, m_BindingSet,
            extraBindingSet, m_Scene->GetDescriptorTable(), &unusedPushConstants, sizeof(unusedPushConstants));

        commandList->endMarker();
    }
    void RtxdiRenderer::_CreateRTBindingLayout()
    {
        //Create binding layouts
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::All;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_StructuredBuffer_SRV(21),		//t_LightDataBuffer
            RHIBindingLayoutItem::RT_TypedBuffer_SRV(22),			//t_NeighborOffsets
            RHIBindingLayoutItem::RT_TypedBuffer_SRV(23),			//t_LightIndexMappingBuffer
            RHIBindingLayoutItem::RT_Texture_SRV(25),				//t_LocalLightPdfTexture
            RHIBindingLayoutItem::RT_StructuredBuffer_SRV(26),		//t_GeometryInstanceToLight

            RHIBindingLayoutItem::RT_StructuredBuffer_UAV(13),		//u_LightReservoirs
            RHIBindingLayoutItem::RT_StructuredBuffer_UAV(14),		//u_GIReservoirs
            RHIBindingLayoutItem::RT_TypedBuffer_UAV(15),			//u_RisBuffer
            RHIBindingLayoutItem::RT_TypedBuffer_UAV(16),			//u_RisLightDataBuffer

            RHIBindingLayoutItem::RT_VolatileConstantBuffer(5),	//g_RtxdiBridgeConst

            RHIBindingLayoutItem::RT_Sampler(4)
        };
        m_BindingLayout = m_Device->createBindingLayout(layoutDesc);

      //  m_RtxdiConstantBuffer = m_Device->createBuffer(RHIUtils::CreateVolatileConstantBufferDesc(sizeof(RtxdiBridgeConstants), "RtxdiBridgeConstants", 16));
    }
    void RtxdiRenderer::_CreateRTBindingSets()
    {
        for (int currentFrame = 0; currentFrame <= 1; currentFrame++)
        {
            BindingSetDesc bindingSetDesc;
            bindingSetDesc.bindings = {
                // RTXDI resources
                BindingSetItem::StructuredBuffer_SRV(21, m_RtxdiResources->LightDataBuffer),
                BindingSetItem::TypedBuffer_SRV(22, m_RtxdiResources->NeighborOffsetsBuffer),
                BindingSetItem::TypedBuffer_SRV(23, m_RtxdiResources->LightIndexMappingBuffer),
                BindingSetItem::Texture_SRV(25, m_RtxdiResources->LocalLightPdfTexture),
                BindingSetItem::StructuredBuffer_SRV(26, m_RtxdiResources->GeometryInstanceToLightBuffer),

                // Render targets
                BindingSetItem::StructuredBuffer_UAV(13, m_RtxdiResources->LightReservoirBuffer),
                BindingSetItem::StructuredBuffer_UAV(14, m_RtxdiResources->GIReservoirBuffer),
                BindingSetItem::TypedBuffer_UAV(15, m_RtxdiResources->RisBuffer),
                BindingSetItem::TypedBuffer_UAV(16, m_RtxdiResources->RisLightDataBuffer),

                BindingSetItem::ConstantBuffer(5, m_RtxdiConstantBuffer),

                BindingSetItem::Sampler(4, m_CommonPasses->m_LinearWrapSampler)
            };

            const BindingSetHandle bindingSet = m_Device->createBindingSet(bindingSetDesc, m_BindingLayout);
            if (currentFrame)
                m_BindingSet = bindingSet;
            else
                m_PrevBindingSet = bindingSet;
        }
    }
}
