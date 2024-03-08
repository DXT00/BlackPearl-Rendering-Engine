#include "pch.h"
#include "VkDevice.h"

namespace BlackPearl {
	Device::Device(const DeviceDesc& desc)
	{
	}

	Device::~Device()
	{
	}

	FramebufferHandle Device::createFramebuffer(const FramebufferDesc& desc)
	{
		return FramebufferHandle();
	}

	GraphicsPipelineHandle Device::createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb)
	{
		return GraphicsPipelineHandle();
	}

	ComputePipelineHandle Device::createComputePipeline(const ComputePipelineDesc& desc)
	{
		return ComputePipelineHandle();
	}

	MeshletPipelineHandle Device::createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb)
	{
		return MeshletPipelineHandle();
	}

}
