#include "pch.h"
#include "OpenGLDevice.h"
#include "OpenGLUtil.h"
#include "OpenGLSampler.h"
namespace BlackPearl {
	Sampler::~Sampler()
	{

	}

	SamplerHandle Device::createSampler(const SamplerDesc& d)
	{
		Sampler* sampler = new Sampler();

		const bool anisotropyEnable = d.maxAnisotropy > 1.0f;


		sampler->desc = d;


		sampler->samplerState = new FOpenGLSamplerState;

		sampler->samplerState->Data.WrapS = OpenGLUtil::convertSamplerAddressMode(d.addressU);
		sampler->samplerState->Data.WrapT = OpenGLUtil::convertSamplerAddressMode(d.addressV);
		sampler->samplerState->Data.WrapR = OpenGLUtil::convertSamplerAddressMode(d.addressW);
		sampler->samplerState->Data.LODBias = d.mipBias;

		sampler->samplerState->Data.MaxAnisotropy = d.maxAnisotropy > 1.0 ? d.maxAnisotropy : 1;
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
			sampler->samplerState->Data.CompareMode = GL_COMPARE_REF_TO_TEXTURE;
			sampler->samplerState->Data.CompareFunc = GL_LESS;
		}
		else
		{
			sampler->samplerState->Data.CompareMode = GL_NONE;
		}

		/*	if (OpenGLConsoleVariables::GOpenGLForceBilinear && (SamplerState->Data.MinFilter == GL_LINEAR_MIPMAP_LINEAR))
			{
				SamplerState->Data.MinFilter = GL_LINEAR_MIPMAP_NEAREST;
			}*/

		sampler->samplerState->Resource = 0;

		//FRHICommandListImmediate::Get().EnqueueLambda([SamplerState](FRHICommandListImmediate&)
		{
			//VERIFY_GL_SCOPE();
			FOpenGL::GenSamplers(1, &sampler->samplerState->Resource);

			FOpenGL::SetSamplerParameter(sampler->samplerState->Resource, GL_TEXTURE_WRAP_S, sampler->samplerState->Data.WrapS);
			FOpenGL::SetSamplerParameter(sampler->samplerState->Resource, GL_TEXTURE_WRAP_T, sampler->samplerState->Data.WrapT);
			if (FOpenGL::SupportsTexture3D())
			{
				FOpenGL::SetSamplerParameter(sampler->samplerState->Resource, GL_TEXTURE_WRAP_R, sampler->samplerState->Data.WrapR);
			}
			if (FOpenGL::SupportsTextureLODBias())
			{
				FOpenGL::SetSamplerParameter(sampler->samplerState->Resource, GL_TEXTURE_LOD_BIAS, sampler->samplerState->Data.LODBias);
			}

			FOpenGL::SetSamplerParameter(sampler->samplerState->Resource, GL_TEXTURE_MIN_FILTER, sampler->samplerState->Data.MinFilter);
			FOpenGL::SetSamplerParameter(sampler->samplerState->Resource, GL_TEXTURE_MAG_FILTER, sampler->samplerState->Data.MagFilter);
			if (FOpenGL::SupportsTextureFilterAnisotropic())
			{
				FOpenGL::SetSamplerParameter(sampler->samplerState->Resource, GL_TEXTURE_MAX_ANISOTROPY_EXT, sampler->samplerState->Data.MaxAnisotropy);
			}

			if (FOpenGL::SupportsTextureCompare())
			{
				FOpenGL::SetSamplerParameter(sampler->samplerState->Resource, GL_TEXTURE_COMPARE_MODE, sampler->samplerState->Data.CompareMode);
				FOpenGL::SetSamplerParameter(sampler->samplerState->Resource, GL_TEXTURE_COMPARE_FUNC, sampler->samplerState->Data.CompareFunc);
			}
			//});

		// Manually add reference as we control the creation/destructions, TODO::
		/*sampler->samplerState->AddRef();
		GSamplerStateCache.Add(Initializer, sampler->samplerState);*/

		//return SamplerState;

			return SamplerHandle(sampler);
		}
	}