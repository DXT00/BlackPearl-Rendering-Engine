#include "pch.h"
#include "VkPipeline.h"

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