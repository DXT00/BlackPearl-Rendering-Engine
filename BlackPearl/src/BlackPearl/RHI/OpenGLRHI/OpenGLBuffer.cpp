#include "pch.h"
#include "OpenGLBuffer.h"
#include "glad/glad.h"

namespace BlackPearl {
    OpenGLRenderBuffer::OpenGLRenderBuffer(BufferDesc _desc):
        OpenGLBuffer(desc)
    {
        glGenRenderbuffers(1, &rbo);

    }
}
