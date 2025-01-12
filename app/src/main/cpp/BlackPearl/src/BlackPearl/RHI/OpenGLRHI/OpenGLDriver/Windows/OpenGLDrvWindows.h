// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

//#include COMPILED_PLATFORM_HEADER(OpenGLThirdParty.h)

#ifdef GE_PLATFORM_WINDOWS
#include "glad/glad.h"
#include <windef.h>
#include <wingdi.h>
#include "GL/wglext.h"
#else
//TODO:: Andriod gles lib
#endif
