// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

//#include "CoreTypes.h"
//#include "Misc/Timespan.h"
//#include "HAL/PlatformMemory.h"
#include "BlackPearl/Core/Platform.h"
#include <mutex>

class FString;

/**
 * This is the Windows version of a critical section. It uses an aggregate
 * CRITICAL_SECTION to implement its locking.
 */
class FWindowsCriticalSection
{
public:
	FWindowsCriticalSection(const FWindowsCriticalSection&) = delete;
	FWindowsCriticalSection& operator=(const FWindowsCriticalSection&) = delete;

	/**
	 * Constructor that initializes the aggregated critical section
	 */
	FORCEINLINE FWindowsCriticalSection()
	{
		/*CA_SUPPRESS(28125);
		Windows::InitializeCriticalSection(&CriticalSection);
		Windows::SetCriticalSectionSpinCount(&CriticalSection,4000);*/
	}

	/**
	 * Destructor cleaning up the critical section
	 */
	FORCEINLINE ~FWindowsCriticalSection()
	{
		//Windows::DeleteCriticalSection(&CriticalSection);
	}

	/**
	 * Locks the critical section
	 */
	FORCEINLINE void Lock()
	{
		Mutex.lock();
		//Windows::EnterCriticalSection(&CriticalSection);
	}

	/**
	 * Attempt to take a lock and returns whether or not a lock was taken.
	 *
	 * @return true if a lock was taken, false otherwise.
	 */
	FORCEINLINE bool TryLock()
	{
		/*if (Windows::TryEnterCriticalSection(&CriticalSection))
		{
			return true;
		}
		return false;*/
		return Mutex.try_lock();

	}

	/**
	 * Releases the lock on the critical section
	 * 
	 * Calling this when not locked is undefined behavior & may cause indefinite waiting on next lock.
	 * See: https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-leavecriticalsection#remarks
	 */
	FORCEINLINE void Unlock()
	{
		Mutex.unlock();
		//Windows::LeaveCriticalSection(&CriticalSection);
	}

private:
	/**
	 * The windows specific critical section
	 */
	//Windows::CRITICAL_SECTION CriticalSection;
	std::mutex Mutex;


};
typedef FWindowsCriticalSection FCriticalSection;
