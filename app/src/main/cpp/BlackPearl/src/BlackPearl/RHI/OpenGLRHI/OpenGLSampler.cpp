#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLDevice.h"
#include "RHI/OpenGLRHI/OpenGLUtil.h"
#include "RHI/OpenGLRHI/OpenGLSampler.h"
namespace BlackPearl {
	Sampler::~Sampler()
	{

	}

	SamplerHandle Device::createSampler(const SamplerDesc& d)
	{
		Sampler* sampler = new Sampler(d);

		
		return SamplerHandle(sampler);
		
	}
	Sampler::Sampler(const SamplerDesc& d)
	{
		const bool anisotropyEnable = d.maxAnisotropy > 1.0f;


		desc = d;

		samplerState = new FOpenGLSamplerState;

		samplerState->Data.WrapS = OpenGLUtil::convertSamplerAddressMode(d.addressU);
		samplerState->Data.WrapT = OpenGLUtil::convertSamplerAddressMode(d.addressV);
		samplerState->Data.WrapR = OpenGLUtil::convertSamplerAddressMode(d.addressW);
		samplerState->Data.LODBias = d.mipBias;

		samplerState->Data.MaxAnisotropy = d.maxAnisotropy > 1.0 ? d.maxAnisotropy : 1;
		//TODO::
		const bool bComparisonEnabled = false;//(Initializer.SamplerComparisonFunction != SCF_Never);

		//switch (Initializer.Filter)
		//{
		//case SF_AnisotropicPoint:
		//	// This is set up like this in D3D11, so following suit.
		//	// Otherwise we're getting QA reports about weird artifacting, because QA scenes are set up in
		//	// D3D11 and AnisotropicPoint when Linear would be proper goes unnoticed there.

		//	// Once someone decides to fix things in D3D11, I assume they'll look here to fix things up too. The code below is waiting.

		//	// MagFilter	= GL_NEAREST;
		//	// MinFilter	= bComparisonEnabled ? GL_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
		//	// break;

		//	// PASS-THROUGH to AnisotropicLinear!

		//case SF_AnisotropicLinear:
		//	SamplerState->Data.MagFilter = GL_LINEAR;
		//	SamplerState->Data.MinFilter = bComparisonEnabled ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR;
		//	SamplerState->Data.MaxAnisotropy = FMath::Min<uint32>(ComputeAnisotropyRT(Initializer.MaxAnisotropy), GMaxOpenGLTextureFilterAnisotropic);
		//	break;
		//case SF_Trilinear:
		//	SamplerState->Data.MagFilter = GL_LINEAR;
		//	SamplerState->Data.MinFilter = bComparisonEnabled ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR;
		//	break;
		//case SF_Bilinear:
		//	SamplerState->Data.MagFilter = GL_LINEAR;
		//	SamplerState->Data.MinFilter = GL_LINEAR_MIPMAP_NEAREST;
		//	break;
		//default:
		//case SF_Point:
		//	SamplerState->Data.MagFilter = GL_NEAREST;
		//	SamplerState->Data.MinFilter = GL_NEAREST_MIPMAP_NEAREST;
		//	break;
		//}

		if (bComparisonEnabled)
		{
			//assert(Initializer.SamplerComparisonFunction == SCF_Less);
			samplerState->Data.CompareMode = GL_COMPARE_REF_TO_TEXTURE;
			samplerState->Data.CompareFunc = GL_LESS;
		}
		else
		{
			samplerState->Data.CompareMode = GL_NONE;
		}
		samplerState->Data.MinFilter = OpenGLUtil::convertTextureFilter(desc.minFilter);
		samplerState->Data.MagFilter = OpenGLUtil::convertTextureFilter(desc.magFilter);


		/*	if (OpenGLConsoleVariables::GOpenGLForceBilinear && (SamplerState->Data.MinFilter == GL_LINEAR_MIPMAP_LINEAR))
			{
				SamplerState->Data.MinFilter = GL_LINEAR_MIPMAP_NEAREST;
			}*/

		samplerState->Resource = 0;

		//FRHICommandListImmediate::Get().EnqueueLambda([SamplerState](FRHICommandListImmediate&)
		//{
			//VERIFY_GL_SCOPE();
			FOpenGL::GenSamplers(1, &samplerState->Resource);

			FOpenGL::SetSamplerParameter(samplerState->Resource, GL_TEXTURE_WRAP_S, samplerState->Data.WrapS);
			FOpenGL::SetSamplerParameter(samplerState->Resource, GL_TEXTURE_WRAP_T, samplerState->Data.WrapT);
			if (FOpenGL::SupportsTexture3D())
			{
				FOpenGL::SetSamplerParameter(samplerState->Resource, GL_TEXTURE_WRAP_R, samplerState->Data.WrapR);
			}
			if (FOpenGL::SupportsTextureLODBias())
			{
				FOpenGL::SetSamplerParameter(samplerState->Resource, GL_TEXTURE_LOD_BIAS, samplerState->Data.LODBias);
			}

			FOpenGL::SetSamplerParameter(samplerState->Resource, GL_TEXTURE_MIN_FILTER, samplerState->Data.MinFilter);
			FOpenGL::SetSamplerParameter(samplerState->Resource, GL_TEXTURE_MAG_FILTER, samplerState->Data.MagFilter);
			if (FOpenGL::SupportsTextureFilterAnisotropic())
			{
				FOpenGL::SetSamplerParameter(samplerState->Resource, GL_TEXTURE_MAX_ANISOTROPY_EXT, samplerState->Data.MaxAnisotropy);
			}

			if (FOpenGL::SupportsTextureCompare())
			{
				FOpenGL::SetSamplerParameter(samplerState->Resource, GL_TEXTURE_COMPARE_MODE, samplerState->Data.CompareMode);
				FOpenGL::SetSamplerParameter(samplerState->Resource, GL_TEXTURE_COMPARE_FUNC, samplerState->Data.CompareFunc);
			}
			//});

		// Manually add reference as we control the creation/destructions, TODO::
		/*samplerState->AddRef();
		GSamplerStateCache.Add(Initializer, samplerState);*/

		//return SamplerState;

	}
}