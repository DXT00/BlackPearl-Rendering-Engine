#pragma once
#if GE_API_VULKAN

#include "../RefCountPtr.h"
#include"../RHIPipeline.h"
#include "../RHIBindingLayoutDesc.h"
#include "../RHIBindingSet.h"
#include "VkContext.h"
#include "VkBindingLayout.h"
#include <vulkan/vulkan_core.h>
namespace BlackPearl {
	class GraphicsPipeline :public RefCounter<IGraphicsPipeline>
	{
	public:

		GraphicsPipelineDesc desc;
		FramebufferInfo framebufferInfo;
		ShaderType shaderMask = ShaderType::None;
		std::vector<RefCountPtr<BindingLayout>> pipelineBindingLayouts;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		VkShaderStageFlags pushConstantVisibility;
		bool usesBlendConstants = false;

		explicit GraphicsPipeline(const VulkanContext& context)
			: m_Context(context)
		{ }

		~GraphicsPipeline() override;
		const GraphicsPipelineDesc& getDesc() const override { return desc; }
		const FramebufferInfo& getFramebufferInfo() const override { return framebufferInfo; }

	private:
		const VulkanContext& m_Context;
	};

	class ComputePipeline :public RefCounter<IComputePipeline>
	{
	public:
		ComputePipelineDesc desc;

		std::vector<RefCountPtr<BindingLayout>> pipelineBindingLayouts;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		VkShaderStageFlags pushConstantVisibility;

		explicit ComputePipeline(const VulkanContext& context)
			: m_Context(context)
		{ }

		~ComputePipeline() override;
		const ComputePipelineDesc& getDesc() const override { return desc; }

	private:
		const VulkanContext& m_Context;

	};
	class MeshletPipeline : public RefCounter<IMeshletPipeline>
	{
	public:
		MeshletPipelineDesc desc;
		FramebufferInfo framebufferInfo;
		ShaderType shaderMask = ShaderType::None;
		std::vector<RefCountPtr<BindingLayout>> pipelineBindingLayouts;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		VkShaderStageFlags pushConstantVisibility;
		bool usesBlendConstants = false;

		explicit MeshletPipeline(const VulkanContext& context)
			: m_Context(context)
		{ }

		~MeshletPipeline() override;
		const MeshletPipelineDesc& getDesc() const override { return desc; }
		const FramebufferInfo& getFramebufferInfo() const override { return framebufferInfo; }

	private:
		const VulkanContext& m_Context;
	};


	class RayTracingPipeline :public RefCounter<IRayTracingPipeline>
	{
	public:
		RayTracingPipelineDesc desc;
		std::vector <RefCountPtr<BindingLayout>> pipelineBindingLayouts;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		VkShaderStageFlags pushConstantVisibility;

		std::unordered_map<std::string, uint32_t> shaderGroups; // name -> index
		std::vector<uint8_t> shaderGroupHandles;

		explicit RayTracingPipeline(const VulkanContext& context)
			: m_Context(context)
		{ }

		~RayTracingPipeline() override;
		const RayTracingPipelineDesc& getDesc() const override { return desc; }
		ShaderTableHandle createShaderTable() override;

		int findShaderGroup(const std::string& name); // returns -1 if not found

	private:
		const VulkanContext& m_Context;

	};
}
#endif

