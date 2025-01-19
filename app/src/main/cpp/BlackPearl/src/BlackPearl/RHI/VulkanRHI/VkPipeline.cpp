#include "pch.h"
#if GE_API_VULKAN

#include "VkPipeline.h"
#include "OpenGLPipeline.h"

namespace BlackPearl {
	//TODO::
	ComputePipeline::~ComputePipeline()
	{
        
	}
	GraphicsPipeline::~GraphicsPipeline()
	{
	}

	MeshletPipeline::~MeshletPipeline()
	{
	}
	RayTracingPipeline::~RayTracingPipeline()
	{
	}
	ShaderTableHandle RayTracingPipeline::createShaderTable()
	{
		return ShaderTableHandle();
	}
	int RayTracingPipeline::findShaderGroup(const std::string& name)
	{
		return 0;
	}
}
#endif