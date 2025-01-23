#include "pch.h"
#include "OpenGLPipeline.h"
#include "OpenGLBuffer.h"
#include "OpenGLBufferResource.h"
#include "OpenGLDevice.h"
#include "OpenGLShader.h"
#include "OpenGLInputLayout.h"

namespace BlackPearl {

	void GraphicsPipeline::getNumUniformBuffers(uint32_t** unifromBuffers)
	{
	}


	GraphicsPipelineHandle Device::createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb)
	{
		Framebuffer* fb = dynamic_cast<Framebuffer*>(fb);

		InputLayout* inputLayout = dynamic_cast<InputLayout*>(desc.inputLayout.Get());

		GraphicsPipeline* pso = new GraphicsPipeline(m_Context);
		pso->desc = desc;
		pso->framebufferInfo = fb->framebufferInfo;

		for (const BindingLayoutHandle& _layout : desc.bindingLayouts)
		{
			BindingLayout* layout = dynamic_cast<BindingLayout*>(_layout.Get());
			pso->pipelineBindingLayouts.push_back(layout);
		}

		Shader* VS = dynamic_cast<Shader*>(desc.VS.Get());
		Shader* HS = dynamic_cast<Shader*>(desc.HS.Get());
		Shader* DS = dynamic_cast<Shader*>(desc.DS.Get());
		Shader* GS = dynamic_cast<Shader*>(desc.GS.Get());
		Shader* PS = dynamic_cast<Shader*>(desc.PS.Get());

		size_t numShaders = 0;
		size_t numShadersWithSpecializations = 0;
		size_t numSpecializationConstants = 0;

		//// Count the spec constants for all stages
		//VkUtil::countSpecializationConstants(VS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
		//VkUtil::countSpecializationConstants(HS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
		//VkUtil::countSpecializationConstants(DS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
		//VkUtil::countSpecializationConstants(GS, numShaders, numShadersWithSpecializations, numSpecializationConstants);
		//VkUtil::countSpecializationConstants(PS, numShaders, numShadersWithSpecializations, numSpecializationConstants);



		// Set up shader stages
		if (desc.VS)
		{
			pso->shaderMask = pso->shaderMask | ShaderType::Vertex;
		}

		if (desc.HS)
		{
			pso->shaderMask = pso->shaderMask | ShaderType::Hull;
		}

		if (desc.DS)
		{
			pso->shaderMask = pso->shaderMask | ShaderType::Domain;
		}

		if (desc.GS)
		{
			pso->shaderMask = pso->shaderMask | ShaderType::Geometry;
		}

		if (desc.PS)
		{	
			pso->shaderMask = pso->shaderMask | ShaderType::Pixel;
		}

		//// set up vertex input state
		//VkPipelineVertexInputStateCreateInfo vertexInput{};
		//if (inputLayout)
		//{
		//	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		//	vertexInput.vertexBindingDescriptionCount = uint32_t(inputLayout->bindingDesc.size());
		//	vertexInput.pVertexBindingDescriptions = inputLayout->bindingDesc.data();
		//	vertexInput.vertexAttributeDescriptionCount = uint32_t(inputLayout->attributeDesc.size());
		//	vertexInput.pVertexAttributeDescriptions = inputLayout->attributeDesc.data();
		//}

		//VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		//inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		//inputAssembly.topology = VkUtil::convertPrimitiveTopology(desc.primType);
		//inputAssembly.primitiveRestartEnable = VK_FALSE;

		//// fixed function state
		//const auto& rasterState = desc.renderState.rasterState;
		//const auto& depthStencilState = desc.renderState.depthStencilState;
		//const auto& blendState = desc.renderState.blendState;


		//VkPipelineViewportStateCreateInfo viewportState{};
		//viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		//viewportState.viewportCount = 1;
		//viewportState.scissorCount = 1;

		//VkPipelineRasterizationStateCreateInfo rasterizer{};
		//rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		//rasterizer.polygonMode = VkUtil::convertFillMode(rasterState.fillMode);
		//rasterizer.lineWidth = 1.0f;
		//rasterizer.cullMode = VkUtil::convertCullMode(rasterState.cullMode);
		//rasterizer.frontFace = rasterState.frontCounterClockwise ?
		//	VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;// VK_FRONT_FACE_COUNTER_CLOCKWISE;
		//rasterizer.depthBiasEnable = rasterState.depthBias ? true : false;
		//rasterizer.depthBiasConstantFactor = float(rasterState.depthBias);
		//rasterizer.depthBiasClamp = rasterState.depthBiasClamp;
		//rasterizer.depthBiasSlopeFactor = rasterState.slopeScaledDepthBias;

		//// Conservative raster state
		//VkPipelineRasterizationConservativeStateCreateInfoEXT conservativeRasterState{};
		//conservativeRasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT;
		//conservativeRasterState.conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT;

		//if (rasterState.conservativeRasterEnable)
		//{
		//	rasterizer.pNext = &conservativeRasterState;
		//}

		//VkPipelineMultisampleStateCreateInfo multisample{};
		//multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		//multisample.rasterizationSamples = VkSampleCountFlagBits(fb->framebufferInfo.sampleCount);
		//multisample.alphaToCoverageEnable = blendState.alphaToCoverageEnable;


		//VkPipelineDepthStencilStateCreateInfo depthStencil{};

		//depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		//depthStencil.depthTestEnable = depthStencilState.depthTestEnable;
		//depthStencil.depthWriteEnable = depthStencilState.depthWriteEnable;
		//depthStencil.depthCompareOp = VkUtil::convertCompareOp(depthStencilState.depthFunc);
		//depthStencil.stencilTestEnable = depthStencilState.stencilEnable;
		//depthStencil.front = VkUtil::convertStencilState(depthStencilState, depthStencilState.frontFaceStencil);
		//depthStencil.back = VkUtil::convertStencilState(depthStencilState, depthStencilState.backFaceStencil);
		//// VRS state

		//VkFragmentShadingRateCombinerOpKHR combiners[2] =
		//{ VkUtil::convertShadingRateCombiner(desc.shadingRateState.pipelinePrimitiveCombiner),
		//	VkUtil::convertShadingRateCombiner(desc.shadingRateState.imageCombiner)
		//};
		//VkPipelineFragmentShadingRateStateCreateInfoKHR shadingRateState{};
		//shadingRateState.sType = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR;
		//shadingRateState.combinerOps[0] = combiners[0];
		//shadingRateState.combinerOps[1] = combiners[1];
		//shadingRateState.fragmentSize = VkUtil::convertFragmentShadingRate(desc.shadingRateState.shadingRate);

		///*.setCombinerOps(combiners)
		//.setFragmentSize(convertFragmentShadingRate(desc.shadingRateState.shadingRate));*/
		//static_vector<VkDescriptorSetLayout, c_MaxBindingLayouts> descriptorSetLayouts;
		//uint32_t pushConstantSize = 0;
		//for (const BindingLayoutHandle& _layout : desc.bindingLayouts)
		//{
		//	BindingLayout* layout = dynamic_cast<BindingLayout*>(_layout.Get());
		//	descriptorSetLayouts.push_back(layout->descriptorSetLayout);

		//	if (!layout->isBindless)
		//	{
		//		for (const RHIBindingLayoutItem& item : layout->desc.bindings)
		//		{
		//			if (item.type == RHIResourceType::RT_PushConstants)
		//			{
		//				pushConstantSize = item.size;
		//				// assume there's only one push constant item in all layouts -- the validation layer makes sure of that
		//				break;
		//			}
		//		}
		//	}
		//}
		//VkPushConstantRange pushConstantRange{};

		//pushConstantRange.offset = 0;
		//pushConstantRange.size = pushConstantSize;
		//pushConstantRange.stageFlags = VkUtil::convertShaderTypeToShaderStageFlagBits(pso->shaderMask);

		//VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		//pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		//pipelineLayoutInfo.setLayoutCount = uint32_t(descriptorSetLayouts.size());
		//pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		//pipelineLayoutInfo.pushConstantRangeCount = pushConstantSize ? 1 : 0;
		//pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		//if (vkCreatePipelineLayout(m_Context.device, &pipelineLayoutInfo,
		//	m_Context.allocationCallbacks,
		//	&pso->pipelineLayout) != VK_SUCCESS) {
		//	throw std::runtime_error("failed to create pipeline layout!");
		//}

		//attachment_vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(fb->desc.colorAttachments.size());

		//for (uint32_t i = 0; i < uint32_t(fb->desc.colorAttachments.size()); i++)
		//{
		//	colorBlendAttachments[i] = VkUtil::convertBlendState(blendState.targets[i]);
		//}

		//VkPipelineColorBlendStateCreateInfo colorBlend{};
		//colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		//colorBlend.attachmentCount = uint32_t(colorBlendAttachments.size());
		//colorBlend.pAttachments = colorBlendAttachments.data();

		//pso->usesBlendConstants = blendState.usesConstantColor(uint32_t(fb->desc.colorAttachments.size()));

		//VkDynamicState dynamicStates[4] = {
		//	VK_DYNAMIC_STATE_VIEWPORT,
		//	VK_DYNAMIC_STATE_SCISSOR,
		//	VK_DYNAMIC_STATE_BLEND_CONSTANTS,
		//	VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR
		//};

		//VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		//dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		//dynamicStateInfo.dynamicStateCount = pso->usesBlendConstants ? 3 : 2;
		//dynamicStateInfo.pDynamicStates = dynamicStates;

		//VkGraphicsPipelineCreateInfo pipelineInfo{};
		//pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		//pipelineInfo.stageCount = uint32_t(shaderStages.size());
		//pipelineInfo.pStages = shaderStages.data();
		//pipelineInfo.pVertexInputState = &vertexInput;
		//pipelineInfo.pInputAssemblyState = &inputAssembly;
		//pipelineInfo.pViewportState = &viewportState;
		//pipelineInfo.pRasterizationState = &rasterizer;
		//pipelineInfo.pMultisampleState = &multisample;
		//pipelineInfo.pDepthStencilState = &depthStencil;
		//pipelineInfo.pColorBlendState = &colorBlend;
		//pipelineInfo.pDynamicState = &dynamicStateInfo;
		//pipelineInfo.layout = pso->pipelineLayout;
		//pipelineInfo.renderPass = fb->renderPass;
		//pipelineInfo.subpass = 0;
		//pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		//pipelineInfo.basePipelineIndex = -1;
		//pipelineInfo.pTessellationState = VK_NULL_HANDLE;
		//pipelineInfo.pNext = &shadingRateState;

		//VkPipelineTessellationStateCreateInfo tessellationState{};

		//if (desc.primType == PrimitiveType::PatchList)
		//{
		//	tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		//	tessellationState.patchControlPoints = desc.patchControlPoints;
		//	pipelineInfo.pTessellationState = &tessellationState;
		//}
		//if (vkCreateGraphicsPipelines(m_Context.device, m_Context.pipelineCache,
		//	1, &pipelineInfo,
		//	m_Context.allocationCallbacks,
		//	&pso->pipeline) != VK_SUCCESS) {
		//	throw std::runtime_error("failed to create graphics pipeline!");
		//}

		return GraphicsPipelineHandle::Create(pso);

	}

}