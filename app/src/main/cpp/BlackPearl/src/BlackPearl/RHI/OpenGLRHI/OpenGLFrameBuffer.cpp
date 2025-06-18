#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#include "RHI/OpenGLRHI/OpenGLFrameBuffer.h"
#include "RHI/OpenGLRHI/OpenGLBuffer.h"
#include "RHI/OpenGLRHI/OpenGLUtil.h"
#include "RHI/OpenGLRHI/OpenGLTexture.h"
#include "RHI/OpenGLRHI/OpenGLDevice.h"

#include "Config.h"
#include "Core.h"
#include "RHI/RHIGlobals.h"
#include "RHI/RHIDefinitions.h"
namespace BlackPearl {
    // GL_MAX_DRAW_BUFFERS value
    GLint GMaxOpenGLDrawBuffers = 0;
    Framebuffer::Framebuffer(const FramebufferDesc& _desc)
    {
        desc = _desc;
        glGenFramebuffers(1, &m_Fbo);
        GE_ERROR_JUDGE();

    }
    Framebuffer::~Framebuffer()
    {
    }
    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
        _BindColorAttachments();
        _BindDepthAttachments();
        _BindRenderbuffer();

    }
    void Framebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
    }

    void Framebuffer::BindCubeMapColorAttachments(int attachmentId, int face)
    {
        const auto& attach = desc.cubeMapAttachment;
        Texture* texture = static_cast<Texture*>(attach.texture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentId, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture->GetRendererID(), 0);
    }


    void Framebuffer::_BindColorAttachments()
    {
        for (int i = 0; i < desc.colorAttachments.size();i++) {
            const auto& attach = desc.colorAttachments[i];
            Texture* texture = static_cast<Texture*>(attach.texture);
            if (texture->getDesc().sampleCount > 1) {
                FOpenGL::FramebufferTexture2DMultisample(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, OpenGLUtil::convertTextureDimension(attach.texture->getDesc().dimension), texture->GetRendererID(), texture->getDesc().defaultMipLevel, texture->getDesc().sampleCount);

            }
            else {
                FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, OpenGLUtil::convertTextureDimension(attach.texture->getDesc().dimension), texture->GetRendererID(), 0);

            }
            GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
        }
    }

    void Framebuffer::_BindDepthAttachments()
    {
        const auto& attach = desc.depthAttachment;
        Texture* texture = static_cast<Texture*>(attach.texture);
     
        FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, OpenGLUtil::convertTextureDimension(attach.texture->getDesc().dimension), texture->GetRendererID(), 0);
        GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
    }
    
    void Framebuffer::_BindRenderbuffer()
    {
        OpenGLRenderBuffer* renderbuffer = static_cast<OpenGLRenderBuffer*>(desc.rboAttachment.rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer->rbo);

        // Use a single rbo for both depth and stencil buffer.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, renderbuffer->width, renderbuffer->height);
        FOpenGL::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer->rbo);

        GE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }


    void Device::BindPendingFramebuffer(FOpenGLContextState& ContextState) {
        assert((GMaxRHIFeatureLevel >= ERHIFeatureLevel::SM5) || !PendingState.bFramebufferSetupInvalid);

        if (ContextState.Framebuffer != PendingState.Framebuffer)
        {
            if (PendingState.Framebuffer)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, PendingState.Framebuffer);
                GE_ERROR_JUDGE();

                FOpenGL::ReadBuffer(PendingState.FirstNonzeroRenderTarget >= 0 ? GL_COLOR_ATTACHMENT0 + PendingState.FirstNonzeroRenderTarget : GL_NONE);
                GE_ERROR_JUDGE();
                GLenum err = glGetError();
                //GLenum DrawFramebuffers[c_MaxRenderTargets];
                std::vector< GLenum> DrawFramebuffers;
                const GLint MaxDrawBuffers = GMaxOpenGLDrawBuffers;

                for (int32_t RenderTargetIndex = 0; RenderTargetIndex < MaxDrawBuffers; ++RenderTargetIndex)
                {
                    if(PendingState.RenderTargets[RenderTargetIndex])
                        DrawFramebuffers.push_back(GL_COLOR_ATTACHMENT0 + RenderTargetIndex);
                    //DrawFramebuffers[RenderTargetIndex] = PendingState.RenderTargets[RenderTargetIndex] ? GL_COLOR_ATTACHMENT0 + RenderTargetIndex : GL_NONE;
                }
                //FOpenGL::DrawBuffers(MaxDrawBuffers, DrawFramebuffers);

                FOpenGL::DrawBuffers(DrawFramebuffers.size(), DrawFramebuffers.data());
                GE_ERROR_JUDGE();

            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                FOpenGL::ReadBuffer(GL_BACK);
                GE_ERROR_JUDGE();

                FOpenGL::DrawBuffer(GL_BACK);
                GE_ERROR_JUDGE();

            }
            GE_ERROR_JUDGE();

            ContextState.Framebuffer = PendingState.Framebuffer;
        }
    }

    //切换到Cubemap某一面渲染
    void Device::BindPendingFramebufferTextureTarget(FOpenGLContextState& ContextState)
    {
      

        if (PendingState.SubViewEnabled)
        {

            if (PendingState.Framebuffer)
            {
                if (ContextState.FramebufferTextureID != PendingState.FramebufferTextureID
                   || ContextState.FramebufferTextureTarget != PendingState.FramebufferTextureTarget
                    || ContextState.FramebufferTextureMipLevel != PendingState.FramebufferTextureMipLevel
                    ) 
                {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, PendingState.FramebufferTextureTarget, PendingState.FramebufferTextureID, PendingState.FramebufferTextureMipLevel);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }

            }

            ContextState.FramebufferTextureID = PendingState.FramebufferTextureID;
            ContextState.FramebufferTextureTarget = PendingState.FramebufferTextureTarget;

            ContextState.FramebufferTextureMipLevel = PendingState.FramebufferTextureMipLevel;

            ContextState.SubViewEnabled = PendingState.SubViewEnabled;

        }
    }

  

}