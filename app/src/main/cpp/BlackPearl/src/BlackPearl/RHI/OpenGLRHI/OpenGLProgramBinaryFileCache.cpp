// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLProgramBinaryFileCache.cpp: OpenGL program binary file cache stores/loads a set of binary ogl programs.
=============================================================================*/


#if BP_PLATFORM_ANDROID
#include "Android/AndroidPlatformMisc.h"
#endif
 
//static bool GMemoryMapGLProgramCache = true;
//static FAutoConsoleVariableRef CVarMemoryMapGLProgramCache(
//	TEXT("r.OpenGL.MemoryMapGLProgramCache"),
//	GMemoryMapGLProgramCache,
//	TEXT("If true enabled memory mapping of the GL program binary cache. (default)\n")
//	TEXT("If false then upon opening the binary cache all programs are loaded into memory.\n")
//	TEXT("When enabled this can reduce RSS pressure when combined with program LRU. (see r.OpenGL.EnableProgramLRUCache).")
//	,
//	ECVF_ReadOnly | ECVF_RenderThreadSafe
//);
//
//TAutoConsoleVariable<int32> FOpenGLProgramBinaryCache::CVarPBCEnable(
//	TEXT("r.ProgramBinaryCache.Enable"),
//#if PLATFORM_ANDROID
//	1,	// Enabled by default on Android.
//#else
//	0,
//#endif
//	TEXT("If true, enables binary program cache. Enabled by default only on Android"),
//	ECVF_ReadOnly | ECVF_RenderThreadSafe
//);
//
//TAutoConsoleVariable<int32> FOpenGLProgramBinaryCache::CVarRestartAndroidAfterPrecompile(
//	TEXT("r.ProgramBinaryCache.RestartAndroidAfterPrecompile"),
//	0,
//	TEXT("If true, Android apps will restart after precompiling the binary program cache."),
//	ECVF_ReadOnly | ECVF_RenderThreadSafe
//);
//
//static int32 GMaxBinaryProgramLoadTimeMS = 3;
//static FAutoConsoleVariableRef CVarMaxBinaryProgramLoadTime(
//	TEXT("r.OpenGL.MaxBinaryProgramLoadTime"),
//	GMaxBinaryProgramLoadTimeMS,
//	TEXT("The maximum time per frame to transfer programs from the binary program cache to the GL RHI. in milliseconds.\n")
//	TEXT("default 3ms. Note: Driver compile time for programs may exceed this limit if you're not using the LRU."),
//	ECVF_RenderThreadSafe
//);
//
//static int32 GBinaryCachePeriodicFlushProgramCount = 20;
//static FAutoConsoleVariableRef CVarBinaryCachePeriodicFlushProgramCount(
//	TEXT("r.OpenGL.BinaryCachePeriodicFlushProgramCount"),
//	GBinaryCachePeriodicFlushProgramCount,
//	TEXT("When r.PSOPrecaching is active this value\n")
//	TEXT("is the number of appended programs to accumulate before the cache is flushed to storage."),
//	ECVF_RenderThreadSafe
//);
//
//static int32 GBinaryCacheMMapAfterEveryMB = 50;
//static FAutoConsoleVariableRef CVarBinaryCacheMMapAfterEveryMB(
//	TEXT("r.OpenGL.BinaryCacheMMapAfterEveryMB"),
//	GBinaryCacheMMapAfterEveryMB,
//	TEXT("When r.PSOPrecaching is active this value\n")
//	TEXT("specifies the size program binary cache can grow before it is memory mapped, the mmapped programs replace the allocated programs and potentially frees up memory for unused precached programs."),
//	ECVF_RenderThreadSafe
//);
//
//static int32 GBinaryCacheMaxPermittedSizeMB = 350;
//static FAutoConsoleVariableRef CVarBinaryCacheMaxPermittedSizeMB(
//	TEXT("r.OpenGL.BinaryCacheMaxPermittedSize"),
//	GBinaryCacheMaxPermittedSizeMB,
//	TEXT("When r.PSOPrecaching is active and the binary cache's size is greater\n")
//	TEXT("than this value the cache will be deleted at startup. The precaching cache is rebuilt from empty."),
//	ECVF_RenderThreadSafe
//);

namespace BlackPearl
{
	namespace OpenGL
	{

FOpenGLProgramBinaryCache::FOpenGLProgramBinaryCache(const FString& InCachePathRoot)
	: CachePathRoot(InCachePathRoot)
	, BinaryCacheWriteFileHandle(nullptr)
	, CurrentBinaryFileState(EBinaryFileState::Uninitialized)
{
	ANSICHAR* GLVersion = (ANSICHAR*)glGetString(GL_VERSION);
	ANSICHAR* GLRenderer = (ANSICHAR*)glGetString(GL_RENDERER);
	FString HashString;
	HashString.Append(GLVersion);
	HashString.Append(GLRenderer);

#if PLATFORM_ANDROID
	// FORT-512259:
	// Apparently we can't rely on GL_VERSION alone to assume binary compatibility.
	// Some devices report binary compatibility errors after minor OS updates even though the GL driver version has not changed.
	const FString BuildNumber = FAndroidMisc::GetDeviceBuildNumber();
	HashString.Append(BuildNumber);

	// Optional configrule variable for triggering a rebuild of the cache.
	const FString* ConfigRulesGLProgramKey = FAndroidMisc::GetConfigRulesVariable(TEXT("OpenGLProgramCacheKey"));
	if (ConfigRulesGLProgramKey && !ConfigRulesGLProgramKey->IsEmpty())
	{
		HashString.Append(*ConfigRulesGLProgramKey);
	}
#endif

	FSHAHash VersionHash;
	FSHA1::HashBuffer(TCHAR_TO_ANSI(*HashString), HashString.Len(), VersionHash.Hash);

	CacheSubDir = LegacyShaderPlatformToShaderFormat(GMaxRHIShaderPlatform).ToString() + TEXT("_") + VersionHash.ToString();

	// delete anything from the binary program root that does not match the device string.
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	TArray<FString> FoundFiles;
	IFileManager::Get().FindFiles(FoundFiles, *(CachePathRoot/TEXT("*")), true, true);
	for(FString& FoundFile : FoundFiles)
	{
		const FString FullPath = (CachePathRoot / FoundFile);
		const bool bIsDir = PlatformFile.DirectoryExists(*FullPath);
		if (FoundFile != CacheSubDir || !bIsDir)
		{
			bool bSuccess;
			if(bIsDir)
			{
				bSuccess = PlatformFile.DeleteDirectoryRecursively(*FullPath);
			}
			else
			{
				bSuccess = PlatformFile.DeleteFile(*FullPath);
			}
			UE_LOG(LogRHI, Verbose, TEXT("FOpenGLProgramBinaryCache Deleting %s %s"), bIsDir ? TEXT("dir") : TEXT("file"), *FullPath );
			UE_CLOG(!bSuccess, LogRHI, Warning, TEXT("FOpenGLProgramBinaryCache Failed to delete %s"), *FullPath);
		}
	}
}

FOpenGLProgramBinaryCache::~FOpenGLProgramBinaryCache()
{
#if PLATFORM_ANDROID
	if (FAndroidOpenGL::AreRemoteCompileServicesActive())
	{
		FAndroidOpenGL::StopRemoteCompileServices();
	}
#endif

	if (BinaryCacheWriteFileHandle)
	{
		delete BinaryCacheWriteFileHandle;
	}

	if (OnShaderPipelineCacheOpenedDelegate.IsValid())
	{
		FShaderPipelineCache::GetCacheOpenedDelegate().Remove(OnShaderPipelineCacheOpenedDelegate);
	}

	if (OnShaderPipelineCachePrecompilationCompleteDelegate.IsValid())
	{
		FShaderPipelineCache::GetPrecompilationCompleteDelegate().Remove(OnShaderPipelineCachePrecompilationCompleteDelegate);
	}
};

bool FOpenGLProgramBinaryCache::IsEnabled()
{
	return CachePtr != nullptr;
}

bool FOpenGLProgramBinaryCache::IsBuildingCache()
{
	if (CachePtr != nullptr)
	{
		return CachePtr->IsBuildingCache_internal();
	}
	return false;
}

extern bool IsPrecachingEnabled();

void FOpenGLProgramBinaryCache::Initialize()
{
	check(CachePtr == nullptr);

	if (CVarPBCEnable.GetValueOnAnyThread() == 0)
	{
		UE_LOG(LogRHI, Log, TEXT("FOpenGLProgramBinaryCache disabled by r.ProgramBinaryCache.Enable=0"));
		return;
	}

	if (!FOpenGL::SupportsProgramBinary())
	{
		UE_LOG(LogRHI, Warning, TEXT("FOpenGLProgramBinaryCache disabled as devices does not support program binaries"));
		return;
	}

#if PLATFORM_ANDROID
	if (FOpenGL::HasBinaryProgramRetrievalFailed())
	{
		if (FOpenGL::SupportsProgramBinary())
		{
			UE_LOG(LogRHI, Warning, TEXT("FOpenGLProgramBinaryCache: Device has failed to emit program binary despite SupportsProgramBinary == true. Disabling binary cache."));
			return;
		}
	}
#endif


	FString CacheFolderPathRoot;
#if PLATFORM_ANDROID && USE_ANDROID_FILE
	// @todo Lumin: Use that GetPathForExternalWrite or something?
	extern FString GExternalFilePath;
	CacheFolderPathRoot = GExternalFilePath / TEXT("ProgramBinaryCache");

#else
	CacheFolderPathRoot = FPaths::ProjectSavedDir() / TEXT("ProgramBinaryCache");
#endif

	// Remove entire ProgramBinaryCache folder if -ClearOpenGLBinaryProgramCache is specified on command line
	if (FParse::Param(FCommandLine::Get(), TEXT("ClearOpenGLBinaryProgramCache")))
	{
		UE_LOG(LogRHI, Log, TEXT("ClearOpenGLBinaryProgramCache specified, deleting binary program cache folder: %s"), *CacheFolderPathRoot);
		FPlatformFileManager::Get().GetPlatformFile().DeleteDirectoryRecursively(*CacheFolderPathRoot);
	}

	CachePtr = new FOpenGLProgramBinaryCache(CacheFolderPathRoot);
	UE_LOG(LogRHI, Log, TEXT("Enabling program binary cache dir at %s"), *CachePtr->GetProgramBinaryCacheDir());


	if (IsPrecachingEnabled())
	{
		CachePtr->InitPrecaching();
	}
	else
	{
		// Add delegates for the ShaderPipelineCache precompile.
		UE_LOG(LogRHI, Log, TEXT("FOpenGLProgramBinaryCache will be initialized when ShaderPipelineCache opens its file"));
		CachePtr->OnShaderPipelineCacheOpenedDelegate = FShaderPipelineCache::GetCacheOpenedDelegate().AddRaw(CachePtr, &FOpenGLProgramBinaryCache::OnShaderPipelineCacheOpened);
		CachePtr->OnShaderPipelineCachePrecompilationCompleteDelegate = FShaderPipelineCache::GetPrecompilationCompleteDelegate().AddRaw(CachePtr, &FOpenGLProgramBinaryCache::OnShaderPipelineCachePrecompilationComplete);
	}
}

#if PLATFORM_ANDROID
static int32 GNumRemoteProgramCompileServices = 4;
static FAutoConsoleVariableRef CVarNumRemoteProgramCompileServices(
	TEXT("Android.OpenGL.NumRemoteProgramCompileServices"),
	GNumRemoteProgramCompileServices,
	TEXT("The number of separate processes to make available to compile opengl programs.\n")
	TEXT("0 to disable use of separate processes to precompile Programs\n")
	TEXT("valid range is 1-8 (4 default).")
	,
	ECVF_RenderThreadSafe | ECVF_ReadOnly
);
#endif

