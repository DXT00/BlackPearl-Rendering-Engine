#include "pch.h"
#include "OpenGLDevice.h"
#include "OpenGLTexture.h"
#include "OpenGLCubeMapTexture.h"
#include "OpenGLFrameBuffer.h"
#include "BlackPearl/Core.h"
namespace BlackPearl 
{
	TextureHandle OpenGLDevice::createTexture(TextureDesc& d)
	{
		Texture* texture = nullptr;
		if (d.type == TextureType::CubeMap) {
			texture = DBG_NEW CubeMapTexture(d);
		}
		else {
			texture = DBG_NEW Texture(d);

		}
		GE_ASSERT(texture, "texture is nullptr");
		return TextureHandle::Create(texture);
	}
	FramebufferHandle OpenGLDevice::createFramebuffer(const FramebufferDesc& desc)
	{
		return FramebufferHandle();
	}
}