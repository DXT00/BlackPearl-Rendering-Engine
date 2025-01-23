#include "pch.h"
#include "PipelineStateCache.h"
#include "RHIState.h"

GraphicState* PipelineStateCache::GetAndOrCreateGraphicsPipelineState(RHICommandList& RHICmdList, const FGraphicsPipelineStateInitializer& Initializer, EApplyRendertargetOption ApplyFlags, EPSOPrecacheResult PSOPrecacheResult)
{
	LLM_SCOPE(ELLMTag::PSO);
	ValidateGraphicsPipelineStateInitializer(Initializer);

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST 
	if (ApplyFlags == EApplyRendertargetOption::CheckApply)
	{
		// Catch cases where the state does not match
		FGraphicsPipelineStateInitializer NewInitializer = Initializer;
		RHICmdList.ApplyCachedRenderTargets(NewInitializer);

		int32 AnyFailed = 0;
		AnyFailed |= (NewInitializer.RenderTargetsEnabled != Initializer.RenderTargetsEnabled) << 0;

		if (AnyFailed == 0)
		{
			for (int32 i = 0; i < (int32)NewInitializer.RenderTargetsEnabled; i++)
			{
				AnyFailed |= (NewInitializer.RenderTargetFormats[i] != Initializer.RenderTargetFormats[i]) << 1;
				// as long as RT formats match, the flags shouldn't matter. We only store format-influencing flags in the recorded PSOs, so the check would likely fail.
				//AnyFailed |= (NewInitializer.RenderTargetFlags[i] != Initializer.RenderTargetFlags[i]) << 2;
				if (AnyFailed)
				{
					AnyFailed |= i << 24;
					break;
				}
			}
		}

		AnyFailed |= (NewInitializer.DepthStencilTargetFormat != Initializer.DepthStencilTargetFormat) << 3;
		AnyFailed |= (NewInitializer.DepthStencilTargetFlag != Initializer.DepthStencilTargetFlag) << 4;
		AnyFailed |= (NewInitializer.DepthTargetLoadAction != Initializer.DepthTargetLoadAction) << 5;
		AnyFailed |= (NewInitializer.DepthTargetStoreAction != Initializer.DepthTargetStoreAction) << 6;
		AnyFailed |= (NewInitializer.StencilTargetLoadAction != Initializer.StencilTargetLoadAction) << 7;
		AnyFailed |= (NewInitializer.StencilTargetStoreAction != Initializer.StencilTargetStoreAction) << 8;

		checkf(!AnyFailed, TEXT("GetAndOrCreateGraphicsPipelineState RenderTarget check failed with: %i !"), AnyFailed);
	}
#endif

	// Precache PSOs should never go through here.
	ensure(!Initializer.bPSOPrecache);

	FGraphicsPipelineState* OutCachedState = nullptr;

	bool bWasFound = GGraphicsPipelineCache.Find(Initializer, OutCachedState);
	bool DoAsyncCompile = IsAsyncCompilationAllowed(RHICmdList, Initializer.bFromPSOFileCache);

	if (bWasFound == false)
	{
		bool bWasPSOPrecached = PSOPrecacheResult == EPSOPrecacheResult::Active || PSOPrecacheResult == EPSOPrecacheResult::Complete;

		FPipelineFileCacheManager::CacheGraphicsPSO(GetTypeHash(Initializer), Initializer, bWasPSOPrecached);

		// create new graphics state
		OutCachedState = new FGraphicsPipelineState();
		OutCachedState->Stats = FPipelineFileCacheManager::RegisterPSOStats(GetTypeHash(Initializer));
		if (DoAsyncCompile)
		{
			OutCachedState->CompletionEvent = FGraphEvent::CreateGraphEvent();
		}

		if (!Initializer.bFromPSOFileCache)
		{
			GraphicsPipelineCacheMisses++;
		}

		// If the PSO is still precaching then mark as too late
		if (PSOPrecacheResult == EPSOPrecacheResult::Active)
		{
			PSOPrecacheResult = EPSOPrecacheResult::TooLate;
		}

		bool bPSOPrecache = Initializer.bFromPSOFileCache;
		FGraphEventRef GraphEvent = OutCachedState->CompletionEvent;
		InternalCreateGraphicsPipelineState(Initializer, PSOPrecacheResult, DoAsyncCompile, bPSOPrecache, OutCachedState, RHICmdList.IsImmediate());

		// Add dispatch pre requisite for non precaching jobs only
		//if (GraphEvent.IsValid() && (!bPSOPrecache || !FPSOPrecacheThreadPool::UsePool()))
		if (GraphEvent.IsValid() && !bPSOPrecache)
		{
			check(DoAsyncCompile);
			RHICmdList.AddDispatchPrerequisite(GraphEvent);
		}

		GGraphicsPipelineCache.Add(Initializer, OutCachedState);
	}
	else
	{
		if (!Initializer.bFromPSOFileCache && !OutCachedState->IsComplete())
		{
			if (OutCachedState->PrecompileTask)
			{
				// if this is an in-progress threadpool precompile task then it could be seconds away in the queue.
				// Reissue this task so that it jumps the precompile queue.
				OutCachedState->PrecompileTask->Reschedule(&GPSOPrecacheThreadPool.Get(), EQueuedWorkPriority::Highest);
#if PSO_TRACK_CACHE_STATS
				UE_LOG(LogRHI, Log, TEXT("An incomplete precompile task was required for rendering!"));
#endif
			}
			RHICmdList.AddDispatchPrerequisite(OutCachedState->CompletionEvent);
		}

#if PSO_TRACK_CACHE_STATS
		OutCachedState->AddHit();
#endif
	}

	// return the state pointer
	return OutCachedState;
}
