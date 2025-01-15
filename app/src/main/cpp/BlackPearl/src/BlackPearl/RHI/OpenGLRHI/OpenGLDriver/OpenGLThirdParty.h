// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

//#include COMPILED_PLATFORM_HEADER(OpenGLThirdParty.h)

#ifdef GE_PLATFORM_WINDOWS
#include <GL/glcorearb.h>
#include <GL/glext.h>
#include <GL/wglext.h>
#else
//TODO:: Andriod gles lib
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl31.h>
#endif
