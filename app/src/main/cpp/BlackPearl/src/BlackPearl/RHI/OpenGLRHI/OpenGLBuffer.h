#pragma once
#include "BlackPearl/RHI/RHIBuffer.h"
#include "BlackPearl/RHI/RHIState.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLThirdParty.h"
namespace BlackPearl {

	class Buffer: public RefCounter<IBuffer>, public BufferStateExtension
	{
	public:
		Buffer(const BufferDesc& _desc)
			: BufferStateExtension(_desc) {
			desc = _desc;
		}
		BufferDesc desc;
		GLuint rendererID;
		const BufferDesc& getDesc() const override { return desc; }
		

	};
	class OpenGLBufferFactory {
	public:
		static Buffer* createUniformBuffer(const BufferDesc& _desc);
		static Buffer* createIndexBuffer(const BufferDesc& _desc);
		static Buffer* createVertexBuffer(const BufferDesc& _desc);
		static Buffer* createIndirectBuffer(const BufferDesc& _desc);
		static Buffer* createShaderStorageBuffer(const BufferDesc& _desc);
		static Buffer* createTexelBuffer(const BufferDesc& _desc);
	};


	class OpenGLRenderBuffer : public Buffer
	{
	public:
		OpenGLRenderBuffer(const BufferDesc& _desc);
		unsigned int rbo;
		unsigned int width;
		unsigned int height;

	};

	
}