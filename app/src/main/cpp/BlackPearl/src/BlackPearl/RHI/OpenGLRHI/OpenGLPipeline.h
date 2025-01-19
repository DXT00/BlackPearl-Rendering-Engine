#pragma once
#include"../RHIPipeline.h"
#include "OpenGLContext.h"
#include "OpenGLBindingLayout.h"
namespace BlackPearl {
	class GraphicsPipeline :public RefCounter<IGraphicsPipeline>
	{
	public:

		GraphicsPipelineDesc desc;
		FramebufferInfo framebufferInfo;
		ShaderType shaderMask = ShaderType::None;
		std::vector<RefCountPtr<BindingLayout>> pipelineBindingLayouts;
		//use by gl
		uint32_t shaderLinkProgram = -1;
		//VkPipelineLayout pipelineLayout;
		//VkPipeline pipeline;
		//VkShaderStageFlags pushConstantVisibility;
		bool usesBlendConstants = false;

		explicit GraphicsPipeline(const OpenGLContext& context)
			: m_Context(context)
		{
		}

		~GraphicsPipeline() override;
		const GraphicsPipelineDesc& getDesc() const override { return desc; }
		const FramebufferInfo& getFramebufferInfo() const override { return framebufferInfo; }
		void getNumUniformBuffers(uint32_t** unifromBuffers);
		const OpenGLContext& m_Context;
	};

}

