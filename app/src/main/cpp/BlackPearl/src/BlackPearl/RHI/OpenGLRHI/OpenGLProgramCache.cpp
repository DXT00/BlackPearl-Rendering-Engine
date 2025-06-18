// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLProgramBinaryFileCache.cpp: OpenGL program binary file cache stores/loads a set of binary ogl programs.
=============================================================================*/
#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLProgramCache.h"
#include "OpenGLProgramCache.h"
//
//#if BP_PLATFORM_ANDROID
//#include "Android/AndroidPlatformMisc.h"
//#endif
 
namespace BlackPearl {

void FOpenGLProgramBinaryCache::Initialize()
{
}

FGLProgramCache* FGLProgramCache::Get()
{
    static FGLProgramCache s_Instance;
    return &s_Instance;
}

FOpenGLLinkedProgram* FGLProgramCache::GetGLProgram(FOpenGLProgramKey key)
{
    if (!key.isValid()) {
        return nullptr;

    }
    if (m_ProgramMap.find(key) != m_ProgramMap.end()) {
        return m_ProgramMap[key];
    }

	return nullptr;
}

void FGLProgramCache::AddGLProgram(FOpenGLProgramKey key, FOpenGLLinkedProgram* program)
{
    if (m_ProgramMap.find(key) != m_ProgramMap.end()) {
        GE_CORE_WARN("already has a key %s, replace the old one", key.ToString().c_str());
    }
   
     m_ProgramMap[key] = program;
    

}

FGLProgramCache::~FGLProgramCache()
{
    
}
FGLProgramCache::FGLProgramCache()
{

}
}


