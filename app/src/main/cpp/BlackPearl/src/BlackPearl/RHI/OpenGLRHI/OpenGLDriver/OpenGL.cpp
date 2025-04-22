#include "pch.h"
//#include "OpenGL.h"
#include "OpenGLDrv.h"
#include "BlackPearl/RHI/RHIGlobals.h"
#include "OpenGLDrvPrivate.h"

namespace BlackPearl {

	extern bool GDisableOpenGLDebugOutput;
	GLint FOpenGLBase::MaxTextureImageUnits = -1;
	GLint FOpenGLBase::MaxCombinedTextureImageUnits = -1;
	GLint FOpenGLBase::MaxComputeTextureImageUnits = -1;
	GLint FOpenGLBase::MaxVertexTextureImageUnits = -1;
	GLint FOpenGLBase::MaxGeometryTextureImageUnits = -1;
	GLint FOpenGLBase::MaxVaryingVectors = -1;
	GLint FOpenGLBase::TextureBufferAlignment = -1;
	GLint FOpenGLBase::MaxVertexUniformComponents = -1;
	GLint FOpenGLBase::MaxPixelUniformComponents = -1;
	GLint FOpenGLBase::MaxGeometryUniformComponents = -1;
	bool  FOpenGLBase::bSupportsClipControl = false;
	bool  FOpenGLBase::bSupportsASTC = false;
	bool  FOpenGLBase::bSupportsASTCHDR = false;
	bool  FOpenGLBase::bSupportsSeamlessCubemap = false;
	bool  FOpenGLBase::bSupportsVolumeTextureRendering = false;
	bool  FOpenGLBase::bSupportsTextureFilterAnisotropic = false;
	bool  FOpenGLBase::bSupportsDrawBuffersBlend = false;
	bool  FOpenGLBase::bAmdWorkaround = false;



	void FOpenGLBase::ProcessQueryGLInt()
	{
		LOG_AND_GET_GL_INT(GL_MAX_TEXTURE_IMAGE_UNITS, 0, MaxTextureImageUnits);
		LOG_AND_GET_GL_INT(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, 0, MaxVertexTextureImageUnits);
		LOG_AND_GET_GL_INT(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, 0, MaxComputeTextureImageUnits);
		GET_GL_INT(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0, MaxCombinedTextureImageUnits);
	}

	void FOpenGLBase::ProcessExtensions(const std::string& ExtensionsString)
	{
		ProcessQueryGLInt();

		auto CheckAndSetImageUnits = [](GLint& StageImageUnitsINOUT, GLint Limit, const char* Msg)
			{
				const bool bUnsupported = StageImageUnitsINOUT < Limit;
				//GE_ASSERT(bUnsupported, ("GL RHI requires a minimum {2} texture unit count of {1:d}, this device reports {0:d}."), Msg, Limit, StageImageUnitsINOUT);
				assert(!bUnsupported);
				StageImageUnitsINOUT = Limit;
			};

		static const GLint GLESMaxImageUnitsPerStage = 16; // gles 3 spec is a minimum of 16 per stage. 
		static const GLint MaxCombinedImageUnits = 48;

		//if (IsMobilePlatform(GMaxRHIShaderPlatform))
#ifdef GE_PLATFORM_ANDROID
        {
            // clamp things to the levels that the spec is expecting, check the minimum is supported.
            CheckAndSetImageUnits(MaxTextureImageUnits, GLESMaxImageUnitsPerStage, ("pixel stage"));
            CheckAndSetImageUnits(MaxVertexTextureImageUnits, GLESMaxImageUnitsPerStage, ("vertex stage"));
            CheckAndSetImageUnits(MaxGeometryTextureImageUnits, 0, ("geometry stage")); // gles is not expecting this.
            CheckAndSetImageUnits(MaxComputeTextureImageUnits, GLESMaxImageUnitsPerStage, ("compute stage"));
            CheckAndSetImageUnits(MaxCombinedTextureImageUnits, MaxCombinedImageUnits, ("combined"));
        }
#else
		
		
		if (MaxCombinedTextureImageUnits < MaxCombinedImageUnits) {
			GE_CORE_INFO("GL RHI requires a minimum combined texture unit count of {1}, this device reports {0}.", MaxCombinedImageUnits, MaxCombinedTextureImageUnits);
		}
#endif
		// Check for support for advanced texture compression (desktop and mobile)
		bSupportsASTC = ExtensionsString.find("GL_KHR_texture_compression_astc_ldr")!=std::string::npos;

		bSupportsASTCHDR = bSupportsASTC && ExtensionsString.find("GL_KHR_texture_compression_astc_hdr") != std::string::npos;

		bSupportsSeamlessCubemap = ExtensionsString.find("GL_ARB_seamless_cube_map") != std::string::npos;

		bSupportsTextureFilterAnisotropic = ExtensionsString.find("GL_EXT_texture_filter_anisotropic") != std::string::npos;

		bSupportsDrawBuffersBlend = ExtensionsString.find("GL_ARB_draw_buffers_blend") != std::string::npos;

#if GE_PLATFORM_IOS
		GRHIVendorId = 0x1010;
#else
		std::string VendorName(std::string((const char*)glGetString(GL_VENDOR)));
		if (VendorName.find("ATI ") != std::string::npos)
		{
			GRHIVendorId = 0x1002;
#if GE_PLATFORM_WINDOWS || GE_PLATFORM_LINUX
			bAmdWorkaround = true;
#endif
		}
#if GE_PLATFORM_LINUX
		else if (VendorName.find(("X.Org")))
		{
			GRHIVendorId = 0x1002;
			bAmdWorkaround = true;
		}
#endif
		else if (VendorName.find(("Intel ")) != std::string::npos || VendorName == ("Intel"))
		{
			GRHIVendorId = 0x8086;
#if GE_PLATFORM_WINDOWS || GE_PLATFORM_LINUX
			bAmdWorkaround = true;
#endif
		}
		else if (VendorName.find("NVIDIA ") != std::string::npos)
		{
			GRHIVendorId = 0x10DE;
		}
		else if (VendorName.find("ImgTec") != std::string::npos || VendorName.find("Imagination") != std::string::npos)
		{
			GRHIVendorId = 0x1010;
		}
		else if (VendorName.find("ARM") != std::string::npos)
		{
			GRHIVendorId = 0x13B5;
		}
		else if (VendorName.find("Qualcomm") != std::string::npos)
		{
			GRHIVendorId = 0x5143;
		}

#ifdef GE_PLATFORM_LINUX
		if (GRHIVendorId == 0x0)
		{
			// Try harder for Mesa
			const char* AnsiVersion = (const char*)glGetString(GL_VERSION);
			const char* AnsiRenderer = (const char*)glGetString(GL_RENDERER);
			if (AnsiVersion && AnsiRenderer)
			{
				if (FCStringAnsi::Strstr(AnsiVersion, "Mesa"))
				{
					if (FCStringAnsi::Strstr(AnsiRenderer, "AMD") || FCStringAnsi::Strstr(AnsiRenderer, "ATI"))
					{
						// Radeon
						GRHIVendorId = 0x1002;
						bAmdWorkaround = true;
					}
					else if (FCStringAnsi::Strstr(AnsiRenderer, "Intel"))
					{
						GRHIVendorId = 0x8086;
						bAmdWorkaround = true;
					}
				}
			}

			// If still not detected, show a message box to the user (editor build only) and
			// set GRHIVendorId to something to avoid crashing in check()s later
			if (GRHIVendorId == 0x0)
			{
				if (WITH_EDITOR != 0 && !IsRunningCommandlet() && !FApp::IsUnattended())
				{
					FString GlRenderer(ANSI_TO_TCHAR(AnsiRenderer));
					FText ErrorMessage = FText::Format(LOCTEXT("CannotDetermineGraphicsDriversVendor", "Unknown graphics drivers '{0}' by '{1}' are installed on this system. You may experience visual artifacts and other problems."),
						FText::FromString(GlRenderer), FText::FromString(VendorName));
					FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *ErrorMessage.ToString(),
						*LOCTEXT("CannotDetermineGraphicsDriversVendorTitle", "Cannot determine driver vendor.").ToString());
				}

				GRHIVendorId = 0xFFFF;
				bAmdWorkaround = true;	// be conservative here as well.
			}
		}
#endif // PLATFORM_LINUX

#if GE_PLATFORM_WINDOWS
	/*	auto* CVar = IConsoleManager::Get().FindConsoleVariable(("OpenGL.UseStagingBuffer"));
		if (CVar)
		{
			CVar->Set(false);
		}*/
#endif
#endif // !PLATFORM_IOS

		// Setup CVars that require the RHI initialized
	}

}

