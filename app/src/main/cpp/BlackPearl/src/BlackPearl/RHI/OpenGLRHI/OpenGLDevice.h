#pragma once
#include "../RHIDevice.h"

namespace BlackPearl {
	class OpenGLDevice :public RefCounter<IDevice>
	{
	public:
		virtual TextureHandle createTexture(TextureDesc& d) ;
		virtual FramebufferHandle createFramebuffer(const FramebufferDesc& desc);

	};

}

