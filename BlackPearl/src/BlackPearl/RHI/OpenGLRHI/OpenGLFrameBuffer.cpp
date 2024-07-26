#include "pch.h"
#include "glad/glad.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLBuffer.h"
#include "OpenGLTexture.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Core.h"

namespace BlackPearl {
    OpenGLFramebuffer::OpenGLFramebuffer()
    {
        glGenFramebuffers(1, &m_Fbo);

    }
    OpenGLFramebuffer::~OpenGLFramebuffer()
    {
    }
    void OpenGLFramebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
        _BindColorAttachments();
        _BindDepthAttachments();
        _BindRenderbuffer();

    }
    void OpenGLFramebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
    }

    void OpenGLFramebuffer::BindCubeMapColorAttachments(int attachmentId, int face)
    {
        const auto& attach = desc.cubeMapAttachment;
        Texture* texture = static_cast<Texture*>(attach.texture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentId, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture->GetRendererID(), 0);
    }


    void OpenGLFramebuffer::_BindColorAttachments()
    {
        for (int i = 0; i < desc.colorAttachments.size();i++) {
            //将它附加到当前绑定的帧缓冲对象
            const auto& attach = desc.colorAttachments[i];
            Texture* texture = static_cast<Texture*>(attach.texture);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->GetRendererID(), 0);
            GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
        }
    }

    void OpenGLFramebuffer::_BindDepthAttachments()
    {
        const auto& attach = desc.depthAttachment;
        Texture* texture = static_cast<Texture*>(attach.texture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->GetRendererID(), 0);
        GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
    }
    
    void OpenGLFramebuffer::_BindRenderbuffer()
    {
        OpenGLRenderBuffer* renderbuffer = static_cast<OpenGLRenderBuffer*>(desc.rboAttachment.rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer->rbo);

        // Use a single rbo for both depth and stencil buffer.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, renderbuffer->width, renderbuffer->height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer->rbo);

        GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}