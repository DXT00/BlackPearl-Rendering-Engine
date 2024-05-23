#pragma once
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHICommandList.h"
#include "BlackPearl/Renderer/BindingCache.h"
#include "BlackPearl/Renderer/CommonRenderPass.h"

namespace BlackPearl {

	class PostProcessRenderer: public BasicRenderer {
	public:
        enum class RenderPassType : uint32_t
        {
            Debug_BlendDebugViz,

            MaxCount
        };
        enum class ComputePassType
        {
            Bloom,
            TAA,
            Tonmapping,
            StablePlanesDebugViz,
            RELAXDenoiserPrepareInputs,
            REBLURDenoiserPrepareInputs,
            RELAXDenoiserFinalMerge,
            REBLURDenoiserFinalMerge,
            DummyPlaceholder,
            MaxCount
        };

        struct SampleMiniConstants
        {
            unsigned int params[4];
        };
    public:
		PostProcessRenderer();
		~PostProcessRenderer();

        //PostProcessRenderer(
        //    IDevice* device
        //    //std::shared_ptr<donut::engine::ShaderFactory> shaderFactory,
        //    //std::shared_ptr<donut::engine::CommonRenderPasses> commonPasses
        //    //, std::shared_ptr<engine::FramebufferFactory> colorFramebufferFactory
        //);

        void Init(IDevice* device, std::shared_ptr<ShaderFactory> shaderFactory);

        void Render(ICommandList* commandList,
            RenderPassType passType,
            BufferHandle consts,
            SampleMiniConstants& miniConsts,
            IFramebuffer* targetFramebuffer,
            RenderTargets& renderTargets,
            ITexture* sourceTexture);
       /* void Apply(ICommandList* commandList, RenderPassType passType, nvrhi::BufferHandle consts, SampleMiniConstants& miniConsts, nvrhi::IFramebuffer* targetFramebuffer, RenderTargets& renderTargets, nvrhi::ITexture* sourceTexture);
        void Apply(ICommandList* commandList, ComputePassType passType, nvrhi::BufferHandle consts, SampleMiniConstants& miniConsts, nvrhi::BindingSetHandle bindingSet, nvrhi::BindingLayoutHandle bindingLayout, uint32_t width, uint32_t height);
        void Apply(ICommandList* commandList, ComputePassType passType, int pass, nvrhi::BufferHandle consts, SampleMiniConstants& miniConsts, nvrhi::ITexture* workTexture, RenderTargets& renderTargets, nvrhi::ITexture* sourceTexture);*/

	private:
        DeviceHandle             m_Device;
        std::shared_ptr<CommonRenderPasses> m_CommonPasses;

        ShaderHandle             m_RenderShaders[(uint32_t)RenderPassType::MaxCount];
        GraphicsPipelineHandle   m_RenderPSOs[(uint32_t)RenderPassType::MaxCount];
        ShaderHandle             m_ComputeShaders[(uint32_t)ComputePassType::MaxCount];
        ComputePipelineHandle    m_ComputePSOs[(uint32_t)ComputePassType::MaxCount];

        SamplerHandle            m_PointSampler;
        SamplerHandle            m_LinearSampler;

        BindingLayoutHandle      m_BindingLayoutPS;
        BindingSetHandle         m_BindingSetPS;
        BindingLayoutHandle      m_BindingLayoutCS;
        BindingSetHandle         m_BindingSetCS;

        BindingCache     m_BindingCache;
	};
}