#include "pch.h"
#include "OpenGLBuffer.h"
#include "glad/glad.h"
#include "OpenGLBufferResource.h"

namespace BlackPearl {
    OpenGLRenderBuffer::OpenGLRenderBuffer(const BufferDesc& _desc):
        OpenGLBuffer(desc)
    {
        glGenRenderbuffers(1, &rbo);

    }
 
    OpenGLBuffer* OpenGLBufferFactory::createUniformBuffer(const BufferDesc& _desc)
    {
        return nullptr;
    }
    OpenGLBuffer* OpenGLBufferFactory::createIndexBuffer(const BufferDesc& _desc)
    {
        return nullptr;
    }
    OpenGLBuffer* OpenGLBufferFactory::createVertexBuffer(const BufferDesc& _desc)
    {
        return nullptr;
    }
    OpenGLBuffer* OpenGLBufferFactory::createIndirectBuffer(const BufferDesc& _desc)
    {
        return nullptr;
    }
    OpenGLBuffer* OpenGLBufferFactory::createShaderStorageBuffer(const BufferDesc& _desc)
    {
        return nullptr;
    }
    OpenGLBuffer* OpenGLBufferFactory::createTexelBuffer(const BufferDesc& _desc)
    {
        return nullptr;
    }



    OpenGLUniformBuffer::OpenGLUniformBuffer(const FRHIUniformBufferLayout* InLayout, const BufferDesc& _desc) :
        OpenGLBuffer(_desc)
    {
    }
    void OpenGLUniformBuffer::SetGLUniformBufferParams(GLuint InResource, uint32_t InOffset, uint8_t* InPersistentlyMappedBuffer, uint32_t InAllocatedSize, FOpenGLEUniformBufferData* InEmulatedBuffer, bool bInStreamDraw)
    {
    }
    OpenGLUniformBuffer::~OpenGLUniformBuffer()
    {
    }



}
