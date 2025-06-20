// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

//#include COMPILED_PLATFORM_HEADER(OpenGLThirdParty.h)
#include "BlackPearl/Math/Math.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#ifdef GE_PLATFORM_WINDOWS
#include <GL/glcorearb.h>
#include <GL/wglext.h>
#else
	#error "OpenGLWindows.h included for a platform other than Windows."
#endif
#if !EMULATE_ES31
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGL4.h"
#endif



namespace BlackPearl{


	/** Platform specific OpenGL context. */
	struct FPlatformOpenGLContext
	{
		HWND WindowHandle;
		HDC DeviceContext;
		HGLRC OpenGLContext;
		bool bReleaseWindowOnDestroy;
		int32_t SyncInterval;
		GLuint	ViewportFramebuffer;
		GLuint	VertexArrayObject;	// one has to be generated and set for each context (OpenGL 3.2 Core requirements)
		GLuint	BackBufferResource;
		GLenum	BackBufferTarget;
	};








/** This function is handled separately because it is used to get a real context. */
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

// RenderDoc defines
#define GL_DEBUG_TOOL_EXT                 0x6789
#define GL_DEBUG_TOOL_NAME_EXT            0x678A
#define GL_DEBUG_TOOL_PURPOSE_EXT         0x678B
extern bool GRunningUnderRenderDoc;

class FWindowsOpenGL : public FOpenGL4
{
public:
	static FORCEINLINE void InitDebugContext()
	{
		bDebugContext = glIsEnabled(GL_DEBUG_OUTPUT) != GL_FALSE || GRunningUnderRenderDoc;
	}

	static FORCEINLINE void LabelObject(GLenum Type, GLuint Object, const char* Name)
	{
		if (glObjectLabel && bDebugContext)
		{
			glObjectLabel(Type, Object, -1, Name);
		}
	}

	static FORCEINLINE void PushGroupMarker(const char* Name)
	{
		if (glPushDebugGroup && bDebugContext)
		{
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, strlen(Name), Name);
		}
	}

	static FORCEINLINE void PopGroupMarker()
	{
		if (glPopDebugGroup && bDebugContext)
		{
			glPopDebugGroup();
		}
	}

	static FORCEINLINE bool TexStorage2D(GLenum Target, GLint Levels, GLint InternalFormat, GLsizei Width, GLsizei Height, GLenum Format, GLenum Type, ETextureCreateFlags Flags)
	{
		if (glTexStorage2D != NULL)
		{
			glTexStorage2D(Target, Levels, InternalFormat, Width, Height);
			return true;
		}
		else
		{
			return false;
		}
	}

	static FORCEINLINE bool TexStorage2DMultisample(GLenum Target, GLsizei Samples, GLint InternalFormat, GLsizei Width, GLsizei Height, GLboolean FixedSampleLocations)
	{
		if (glTexStorage2DMultisample != NULL)
		{
			glTexStorage2DMultisample(Target, Samples, InternalFormat, Width, Height, FixedSampleLocations);
			return true;
		}
		else
		{
			return false;
		}
	}

	static FORCEINLINE void TexStorage3D(GLenum Target, GLint Levels, GLint InternalFormat, GLsizei Width, GLsizei Height, GLsizei Depth, GLenum Format, GLenum Type)
	{
		if (glTexStorage3D)
		{
			glTexStorage3D(Target, Levels, InternalFormat, Width, Height, Depth);
		}
		else
		{
			const bool bArrayTexture = Target == GL_TEXTURE_2D_ARRAY || Target == GL_TEXTURE_CUBE_MAP_ARRAY;

			for (uint32_t MipIndex = 0; MipIndex < uint32_t(Levels); MipIndex++)
			{
				glTexImage3D(
					Target,
					MipIndex,
					InternalFormat,
					Math::Max<uint32_t>(1, (Width >> MipIndex)),
					Math::Max<uint32_t>(1, (Height >> MipIndex)),
					(bArrayTexture) ? Depth : Math::Max<uint32_t>(1, (Depth >> MipIndex)),
					0,
					Format,
					Type,
					NULL
				);
			}
		}
	};

	
	static FORCEINLINE void CopyImageSubData(GLuint SrcName, GLenum SrcTarget, GLint SrcLevel, GLint SrcX, GLint SrcY, GLint SrcZ, GLuint DstName, GLenum DstTarget, GLint DstLevel, GLint DstX, GLint DstY, GLint DstZ, GLsizei Width, GLsizei Height, GLsizei Depth)
	{
		glCopyImageSubData(SrcName, SrcTarget, SrcLevel, SrcX, SrcY, SrcZ, DstName, DstTarget, DstLevel, DstX, DstY, DstZ, Width, Height, Depth);
	}

	static FORCEINLINE bool SupportsBufferStorage()
	{
		return glBufferStorage != NULL;
	}

	static FORCEINLINE bool SupportsDepthBoundsTest()
	{
		return glDepthBoundsEXT != NULL;
	}

	static FORCEINLINE bool SupportsBindlessTexture()
	{
		return glGetTextureSamplerHandleARB != NULL;
	}

	static FORCEINLINE void BufferStorage(GLenum Target, GLsizeiptr Size, const void* Data, GLbitfield Flags)
	{
		glBufferStorage(Target, Size, Data, Flags);
	}

	static FORCEINLINE void DepthBounds(GLfloat Min, GLfloat Max)
	{
		glDepthBoundsEXT(Min, Max);
	}

	static FORCEINLINE GLuint64 GetTextureSamplerHandle(GLuint Texture, GLuint Sampler)
	{
		return glGetTextureSamplerHandleARB(Texture, Sampler);
	}

	static FORCEINLINE GLuint64 GetTextureHandle(GLuint Texture)
	{
		return glGetTextureHandleARB(Texture);
	}

	static FORCEINLINE void MakeTextureHandleResident(GLuint64 TextureHandle)
	{
		glMakeTextureHandleResidentARB(TextureHandle);
	}

	static FORCEINLINE void MakeTextureHandleNonResident(GLuint64 TextureHandle)
	{
		glMakeTextureHandleNonResidentARB(TextureHandle);
	}

	static FORCEINLINE void UniformHandleui64(GLint Location, GLuint64 Value)
	{
		glUniformHandleui64ARB(Location, Value);
	}

	static FORCEINLINE bool SupportsProgramBinary() { return glProgramBinary != nullptr; }

	static FORCEINLINE void GetProgramBinary(GLuint Program, GLsizei BufSize, GLsizei* Length, GLenum* BinaryFormat, void* Binary)
	{
		glGetProgramBinary(Program, BufSize, Length, BinaryFormat, Binary);
	}

	static FORCEINLINE void ProgramBinary(GLuint Program, GLenum BinaryFormat, const void* Binary, GLsizei Length)
	{
		glProgramBinary(Program, BinaryFormat, Binary, Length);
	}


};

typedef FWindowsOpenGL FOpenGL;




}
