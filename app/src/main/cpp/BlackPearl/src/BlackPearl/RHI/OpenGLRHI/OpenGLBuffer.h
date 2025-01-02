#pragma once
#include "BlackPearl/RHI/RHIBuffer.h"
#include "BlackPearl/RHI/RHIState.h"

namespace BlackPearl {
	class OpenGLBuffer: public RefCounter<IBuffer>, public BufferStateExtension
	{
	public:
		OpenGLBuffer(BufferDesc _desc)
			: BufferStateExtension(_desc) {
			desc = _desc;
		}
		BufferDesc desc;

		const BufferDesc& getDesc() const override { return desc; }

	};


	class OpenGLRenderBuffer : public OpenGLBuffer
	{
	public:
		OpenGLRenderBuffer(BufferDesc _desc);
		unsigned int rbo;
		unsigned int width;
		unsigned int height;

	};

}