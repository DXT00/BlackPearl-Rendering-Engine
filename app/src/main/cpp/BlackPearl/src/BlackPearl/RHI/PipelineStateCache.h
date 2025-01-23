#pragma once
#include "RHIState.h"
#include "RHICommandList.h"
// Typed caches for compute and graphics
//typedef TDiscardableKeyValueCache< FRHIComputeShader*, FComputePipelineState*> FComputePipelineCache;
//typedef TSharedPipelineStateCache<FGraphicsPipelineStateInitializer, FGraphicsPipelineState*> FGraphicsPipelineCache;
//ComputePipelineCache GComputePipelineCache;
//GraphicsPipelineCache GGraphicsPipelineCache;
//TODO:: –Ú¡–ªØ pso
namespace BlackPearl {

	class GraphicsPipelineStateInitializer
	{
	public:

	};
	class PipelineStateCache
	{
	public:
		static GraphicsState* GetAndOrCreateGraphicsPipelineState(ICommandList& RHICmdList, const GraphicsPipelineStateInitializer& Initializer);

	};


}
