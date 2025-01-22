#include "pch.h"
#include "OpenGLBuffer.h"
#include "glad/glad.h"
#include "OpenGLBufferResource.h"
#include "OpenGLDevice.h"
#include "BlackPearl/Core.h"

namespace BlackPearl {

    BufferHandle Device::createBuffer(const BufferDesc& desc)
    {
        if (desc.isVolatile && desc.maxVersions == 0)
            return nullptr;

        if (desc.isVolatile && !desc.isConstantBuffer)
            return nullptr;

        if (desc.byteSize == 0)
            return nullptr;


        Buffer* buffer = nullptr;
        if (desc.isVertexBuffer) {
            buffer = OpenGLBufferFactory::createVertexBuffer(desc);
        }
        else if (desc.isIndexBuffer) {
            buffer = OpenGLBufferFactory::createIndexBuffer(desc);
        }
        else if (desc.isConstantBuffer) {
            buffer = OpenGLBufferFactory::createUniformBuffer(desc);
        }
        else if (desc.isDrawIndirectArgs) {
            buffer = OpenGLBufferFactory::createIndirectBuffer(desc);
        }
        else if (desc.canHaveUAVs || desc.canHaveRawViews) {
            buffer = OpenGLBufferFactory::createShaderStorageBuffer(desc);
        }
        else if (desc.canHaveTypedViews) {
            //todo::
            buffer = OpenGLBufferFactory::createTexelBuffer(desc);
        }
        else if (desc.canHaveTypedViews && desc.canHaveUAVs) {
            buffer = OpenGLBufferFactory::createShaderStorageBuffer(desc);
        }

    }

    OpenGLRenderBuffer::OpenGLRenderBuffer(const BufferDesc& _desc):
        Buffer(desc)
    {
        glGenRenderbuffers(1, &rbo);

    }
 
    Buffer* OpenGLBufferFactory::createUniformBuffer(const BufferDesc& _desc)
    {
        FRHIUniformBufferLayout* layout = new FRHIUniformBufferLayout();
        return DBG_NEW OpenGLUniformBuffer(layout, _desc);
    }
    Buffer* OpenGLBufferFactory::createIndexBuffer(const BufferDesc& _desc)
    {
        return DBG_NEW IndexBuffer(_desc);
    }
    Buffer* OpenGLBufferFactory::createVertexBuffer(const BufferDesc& _desc)
    {
        return  DBG_NEW VertexBuffer(_desc);
    }
    Buffer* OpenGLBufferFactory::createIndirectBuffer(const BufferDesc& _desc)
    {
        return  DBG_NEW IndirectBuffer(_desc);
    }
    Buffer* OpenGLBufferFactory::createShaderStorageBuffer(const BufferDesc& _desc)
    {
        return DBG_NEW ShaderStorageBuffer(_desc);
    }
    //todo
    Buffer* OpenGLBufferFactory::createTexelBuffer(const BufferDesc& _desc)
    {
        return nullptr;
    }



    OpenGLUniformBuffer::OpenGLUniformBuffer(const FRHIUniformBufferLayout* InLayout, const BufferDesc& _desc)
        :Buffer(_desc)
    {
    }
    void OpenGLUniformBuffer::SetGLUniformBufferParams(GLuint InResource, uint32_t InOffset, uint8_t* InPersistentlyMappedBuffer, uint32_t InAllocatedSize, FOpenGLEUniformBufferData* InEmulatedBuffer, bool bInStreamDraw)
    {
    }
    OpenGLUniformBuffer::~OpenGLUniformBuffer()
    {
    }



}
