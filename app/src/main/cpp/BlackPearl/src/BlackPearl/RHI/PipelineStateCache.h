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
		union
		{
			struct
			{
				uint16_t					Reserved : 14;
				uint16_t					bPSOPrecache : 1;
				uint16_t					bFromPSOFileCache : 1;
			};
			uint16_t						Flags;
		};
	};

	void SetGraphicsPipelineState(ICommandList& RHICmdList, const GraphicsPipelineStateInitializer& Initializer, uint32_t StencilRef);

	class PipelineStateCache
	{
	public:
		static GraphicsState* GetAndOrCreateGraphicsPipelineState(ICommandList& RHICmdList, const GraphicsPipelineStateInitializer& Initializer);

	};


}
