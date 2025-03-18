#include "pch.h"
#include "OpenGLDrv.h"
namespace BlackPearl {
	
	void GetExtensionsString(std::string& ExtensionsString)
	{
		GLint ExtensionCount = 0;
		ExtensionsString = "";
		if (FOpenGL::SupportsIndexedExtensions())
		{
			glGetIntegerv(GL_NUM_EXTENSIONS, &ExtensionCount);
			for (int32_t ExtensionIndex = 0; ExtensionIndex < ExtensionCount; ++ExtensionIndex)
			{
				const char* ExtensionString = FOpenGL::GetStringIndexed(GL_EXTENSIONS, ExtensionIndex);

				ExtensionsString += (" ");
				ExtensionsString += std::string(ExtensionString);
			}
		}
		else
		{
			const char* GlGetStringOutput = (const char*)glGetString(GL_EXTENSIONS);
			if (GlGetStringOutput)
			{
				ExtensionsString += GlGetStringOutput;
				ExtensionsString += (" ");
			}
		}
	}
	void InitDefaultGLContextState(void)
	{
		// NOTE: This function can be called before capabilities setup, so extensions need to be checked directly
		std::string ExtensionsString;
		GetExtensionsString(ExtensionsString);

		// Intel HD4000 under <= 10.8.4 requires GL_DITHER disabled or dithering will occur on any channel < 8bits.
		// No other driver does this but we don't need GL_DITHER on anyway.
		glDisable(GL_DITHER);

		if (FOpenGL::SupportsFramebufferSRGBEnable())
		{
			// Render targets with TexCreate_SRGB should do sRGB conversion like in D3D11
			glEnable(GL_FRAMEBUFFER_SRGB);
		}

		// Engine always expects seamless cubemap, so enable it if available
		if (ExtensionsString.find("GL_ARB_seamless_cube_map")!= std::string::npos)
		{
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		}

#if PLATFORM_WINDOWS || PLATFORM_LINUX
		if (OpenGLConsoleVariables::bUseGlClipControlIfAvailable && ExtensionsString.Contains(TEXT("GL_ARB_clip_control")) && !FOpenGL::IsAndroidGLESCompatibilityModeEnabled())
		{
			FOpenGL::EnableSupportsClipControl();
			glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
		}
#endif

		// optional per platform setup
		FOpenGL::SetupDefaultGLContextState(ExtensionsString);
	}


}