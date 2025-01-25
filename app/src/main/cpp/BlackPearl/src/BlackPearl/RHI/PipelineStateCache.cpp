#include "pch.h"
#include "PipelineStateCache.h"
#include "RHIState.h"
#include "RHICommandList.h"
namespace BlackPearl {


	void SetGraphicsPipelineState(ICommandList& RHICmdList, const GraphicsPipelineStateInitializer& Initializer, uint32_t StencilRef)
	{
#if PLATFORM_USE_FALLBACK_PSO
		RHICmdList.SetGraphicsPipelineState(Initializer, StencilRef, bApplyAdditionalState);
#else
		GraphicsState* PipelineState = PipelineStateCache::GetAndOrCreateGraphicsPipelineState(RHICmdList, Initializer);

		if (PipelineState && !Initializer.bFromPSOFileCache)
		{
//#if PIPELINESTATECACHE_VERIFYTHREADSAFE
//			int32 Result = PipelineState->InUseCount.Increment();
//			check(Result >= 1);
//#endif
			//assert(IsInRenderingThread() || IsInParallelRenderingThread());
			RHICmdList.setGraphicsState(*PipelineState);
		}
#endif
	}


	GraphicsState* PipelineStateCache::GetAndOrCreateGraphicsPipelineState(ICommandList& RHICmdList, const GraphicsPipelineStateInitializer& Initializer)
{
	//LLM_SCOPE(ELLMTag::PSO);
	//ValidateGraphicsPipelineStateInitializer(Initializer);


	// Precache PSOs should never go through here.
	assert(!Initializer.bPSOPrecache);

	GraphicsState* OutCachedState = nullptr;
//
//	bool bWasFound = GGraphicsPipelineCache.Find(Initializer, OutCachedState);
//	bool DoAsyncCompile = IsAsyncCompilationAllowed(RHICmdList, Initializer.bFromPSOFileCache);
//
//	if (bWasFound == false)
//	{
//		bool bWasPSOPrecached = PSOPrecacheResult == EPSOPrecacheResult::Active || PSOPrecacheResult == EPSOPrecacheResult::Complete;
//
//		FPipelineFileCacheManager::CacheGraphicsPSO(GetTypeHash(Initializer), Initializer, bWasPSOPrecached);
//
//		// create new graphics state
//		OutCachedState = new GraphicsState();
//		OutCachedState->Stats = FPipelineFileCacheManager::RegisterPSOStats(GetTypeHash(Initializer));
//		if (DoAsyncCompile)
//		{
//			OutCachedState->CompletionEvent = FGraphEvent::CreateGraphEvent();
//		}
//
//		if (!Initializer.bFromPSOFileCache)
//		{
//			GraphicsPipelineCacheMisses++;
//		}
//
//		// If the PSO is still precaching then mark as too late
//		if (PSOPrecacheResult == EPSOPrecacheResult::Active)
//		{
//			PSOPrecacheResult = EPSOPrecacheResult::TooLate;
//		}
//
//		bool bPSOPrecache = Initializer.bFromPSOFileCache;
//		FGraphEventRef GraphEvent = OutCachedState->CompletionEvent;
//		InternalCreateGraphicsPipelineState(Initializer, PSOPrecacheResult, DoAsyncCompile, bPSOPrecache, OutCachedState, RHICmdList.IsImmediate());
//
//		// Add dispatch pre requisite for non precaching jobs only
//		//if (GraphEvent.IsValid() && (!bPSOPrecache || !FPSOPrecacheThreadPool::UsePool()))
//		if (GraphEvent.IsValid() && !bPSOPrecache)
//		{
//			check(DoAsyncCompile);
//			RHICmdList.AddDispatchPrerequisite(GraphEvent);
//		}
//
//		GGraphicsPipelineCache.Add(Initializer, OutCachedState);
//	}
//	else
//	{
//		if (!Initializer.bFromPSOFileCache && !OutCachedState->IsComplete())
//		{
//			if (OutCachedState->PrecompileTask)
//			{
//				// if this is an in-progress threadpool precompile task then it could be seconds away in the queue.
//				// Reissue this task so that it jumps the precompile queue.
//				OutCachedState->PrecompileTask->Reschedule(&GPSOPrecacheThreadPool.Get(), EQueuedWorkPriority::Highest);
//#if PSO_TRACK_CACHE_STATS
//				UE_LOG(LogRHI, Log, TEXT("An incomplete precompile task was required for rendering!"));
//#endif
//			}
//			RHICmdList.AddDispatchPrerequisite(OutCachedState->CompletionEvent);
//		}
//
//#if PSO_TRACK_CACHE_STATS
//		OutCachedState->AddHit();
//#endif
//	}

	// return the state pointer
	return OutCachedState;
}

}