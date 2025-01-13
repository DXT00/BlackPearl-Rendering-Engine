#include "OpenGLQuery.h"
#include <assert.h>

namespace BlackPearl {
	EventQuery::EventQuery()
	{
		// Initialize the query by issuing an initial event.
		IssueEvent();

		assert(FOpenGL::IsSync(Sync));
	}

	EventQuery::~EventQuery()
	{
		FOpenGL::DeleteSync(Sync);
	}

	void EventQuery::IssueEvent()
	{
		if (Sync)
		{
			FOpenGL::DeleteSync(Sync);
			Sync = UGLsync();
		}
		Sync = FOpenGL::FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		FOpenGL::Flush();

		assert(FOpenGL::IsSync(Sync));
	}

	void EventQuery::WaitForCompletion()
	{


		QUICK_SCOPE_CYCLE_COUNTER(STAT_FOpenGLEventQuery_WaitForCompletion);

		assert(FOpenGL::IsSync(Sync));

		// Wait up to 1/2 second for sync execution
		FOpenGL::EFenceResult Status = FOpenGL::ClientWaitSync(Sync, 0, 500 * 1000 * 1000);

		switch (Status)
		{
		case FOpenGL::FR_AlreadySignaled:
		case FOpenGL::FR_ConditionSatisfied:
			break;

		case FOpenGL::FR_TimeoutExpired:
			UE_LOG(LogRHI, Log, TEXT("Timed out while waiting for GPU to catch up. (500 ms)"));
			break;

		case FOpenGL::FR_WaitFailed:
			UE_LOG(LogRHI, Log, TEXT("Wait on GPU failed in driver"));
			break;

		default:
			UE_LOG(LogRHI, Log, TEXT("Unknown error while waiting on GPU"));
			check(0);
			break;
		}
	}

}
