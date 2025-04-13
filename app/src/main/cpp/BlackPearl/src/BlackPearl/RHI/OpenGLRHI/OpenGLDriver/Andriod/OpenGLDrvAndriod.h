
// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	AndroidEGL.h: Private EGL definitions for Android-specific functionality
=============================================================================*/
#pragma once

#include "BlackPearl/Core/Platform.h"
#include "AndroidOpenGL.h"
//#include "CoreMinimal.h"
//#include "Logging/LogMacros.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl31.h>

struct AndroidESPImpl;
struct ANativeWindow;

#ifndef USE_ANDROID_EGL_NO_ERROR_CONTEXT
#if UE_BUILD_SHIPPING
#define USE_ANDROID_EGL_NO_ERROR_CONTEXT 1
#else
#define USE_ANDROID_EGL_NO_ERROR_CONTEXT 0
#endif
#endif // USE_ANDROID_EGL_NO_ERROR_CONTEXT

//DECLARE_LOG_CATEGORY_EXTERN(LogEGL, Log, All);

namespace BlackPearl{

struct FPlatformOpenGLContext
{
	EGLContext	eglContext;
	GLuint		ViewportFramebuffer;
	EGLSurface	eglSurface;
	GLuint		DefaultVertexArrayObject;
	GLuint		BackBufferResource;
	GLenum		BackBufferTarget;
	GLuint		DummyFrameBuffer;

	FPlatformOpenGLContext()
	{
		Reset();
	}

	void Reset()
	{
		eglContext = EGL_NO_CONTEXT;
		eglSurface = EGL_NO_SURFACE;
		ViewportFramebuffer = 0;
		DefaultVertexArrayObject = 0;
		BackBufferResource = 0;
		BackBufferTarget = 0;
		DummyFrameBuffer = 0;
	}
};



typedef FAndroidOpenGL FOpenGL;



}
