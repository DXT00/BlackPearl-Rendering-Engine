#pragma once
#include <unordered_map>
#include "RHIState.h"
#include "RHICommandList.h"
#include "BlackPearl/Renderer/SceneType.h"
// Typed caches for compute and graphics
//typedef TDiscardableKeyValueCache< FRHIComputeShader*, FComputePipelineState*> FComputePipelineCache;
//typedef TSharedPipelineStateCache<FGraphicsPipelineStateInitializer, FGraphicsPipelineState*> FGraphicsPipelineCache;
//ComputePipelineCache GComputePipelineCache;
//TODO:: 序列化 pso
namespace BlackPearl {
	//等同于 GraphicsPipelineStateInitializer
	union PipelineKey
	{
		struct
		{
			MaterialDomain domain : 3;
			RasterCullMode cullMode : 2;
			bool frontCounterClockwise : 1;
			bool reverseDepth : 1;
		} bits;
		uint32_t value = 0;

		static constexpr size_t Count = 1 << 7;
	};

	std::unordered_map<PipelineKey, GraphicsPipelineHandle> GGraphicsPipelineCache;

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
		static GraphicsPipelineHandle GetAndOrCreateGraphicsPipelineState(DeviceHandle device, const GraphicsPipelineDesc& pipelineDesc, const PipelineKey& Initializer, GraphicsState& psoState);

	private:


	};


}
